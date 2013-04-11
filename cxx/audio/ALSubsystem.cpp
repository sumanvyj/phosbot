#include "ALdllmain.h"
#include "ALSubsystem.h"

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

ALSubsystem::ALSubsystem() {}
//-----------------------------------------------------------------------

ALSubsystem::~ALSubsystem()
{
  _deinit();
}
//-----------------------------------------------------------------------

void ALSubsystem::_init()
{
  if(!mInitialized)
  {
    std::cout<<"AL Subsystem starting up...\n";

    // open default device
    if(!(mDevice = alcOpenDevice(0)))
      throw Exception("Audio device could not be opened.");

    // create context (accept defaults, not passing an attrlist)
    if(!(mContext = alcCreateContext(mDevice, 0)))
      throw Exception("Audio context could not be created.");

    // make this context current, and we're good to go
    alcMakeContextCurrent(mContext);

    // put the listener at the origin
    ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
    ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
    ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
    alListenerfv(AL_POSITION,    ListenerPos);
    alListenerfv(AL_VELOCITY,    ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);

    // generate up to 64 sources
    for(int i = 0; i < 64; ++i)
    {
      ALuint source;
      alGenSources(1,&source);
      if(alGetError() != AL_NO_ERROR)
      {
        break;
      }
      else
      {
        mSources.push_back(source);
        mSourceAvailability[source] = true;
      }
    }

    std::cout<<"Audio Sources Allocated: " << mSources.size() << ".\n";

    // Set up audio loaders
    mAudioLoaders["wav"] = &mWavLoader;
    mAudioLoaders["ogg"] = &mOggLoader;

    std::cout<<"AL Subsystem started up.\n";
    mInitialized = true;
  }
}
//-----------------------------------------------------------------------

void ALSubsystem::_deinit()
{
  if(mInitialized)
  {
    std::cout<<"AL Subsystem shutting down...\n";

    // stop all playing sounds
    for(std::list<SoundPtr>::iterator it = mActiveSounds.begin(); 
      it != mActiveSounds.end(); ++it) 
      (*it)->stop();

    // Any hanging references elsewhere should destruct gracefully, though
    // attempting to use them in any other way will (probably) error
    mActiveSounds.clear();
  
    // free all sound sources
    alDeleteSources(mSources.size(), &mSources[0]);
    mSources.clear();
    mSourceAvailability.clear();

    // clear out buffers
    for(std::map<String, ALuint>::iterator it = mAudioBuffers.begin(); 
      it != mAudioBuffers.end(); ++it)
      alDeleteBuffers(1, &(it->second));
    mAudioBuffers.clear();

    // shut down context and device
    alcMakeContextCurrent(0);
    alcDestroyContext(mContext);
    alcCloseDevice(mDevice);

    std::cout<<"AL Subsystem shut down.\n";
    mInitialized = false;
  }
}
//-----------------------------------------------------------------------

void ALSubsystem::_update(Real delta)
{
  for(std::list<SoundPtr>::iterator it = mActiveSounds.begin(); 
    it != mActiveSounds.end(); ++it)
  {	
    // this really only matters if it's a streaming sound
    (*it)->update();

    // free up any sounds that can be
    if((*it).isDone())
    {
      mSourceAvailability[(*it)->getSource()] = true;
      it = mActiveSounds.erase(it);
    }
  }
}
//-----------------------------------------------------------------------

void ALSubsystem::_endState()
{
  for(std::list<SoundPtr>::iterator it = mActiveSounds.begin(); 
    it != mActiveSounds.end(); ++it)
  {
    // we're free to reuse any sources from sounds that don't have leftover refs
    // (this allows background music or whatever to persist between states)
    if((*it).getRefCount() == 1)
    {
      mSourceAvailability[(*it)->getSource()] = true;
      it = mActiveSounds.erase(it);
    }
  }
}
//-----------------------------------------------------------------------

String ALSubsystem::getName()
{
  return String("ALSubsystem");
}
//-----------------------------------------------------------------------

SoundPtr ALSubsystem::play2D(String name, bool startPaused)
{
  SoundPtr out;

  if(mAudioBuffers.find(name) == mAudioBuffers.end())
  {
    loadSound(name);

    if(mAudioBuffers.find(name) == mAudioBuffers.end())
    {
      std::cout<<"Could not play sound, could not find or load audio.\n";
      return out;
    }
  }

  if(!hasSources())
  {
    std::cout<<"Could not play sound, no available sound sources.\n";
  }
  else
  {
    out.bind(new BufferedSound(getSource(), mAudioBuffers[name], mAudioData[name]));
    mActiveSounds.push_back(out);
    if(!startPaused)
      out->play();
  }

  return out;
}
//-----------------------------------------------------------------------

SoundPtr ALSubsystem::stream2D(String filename, bool startPaused)
{
  SoundPtr out;

  String ext = filename.substr(filename.find_last_of(".") + 1);

  if(mAudioLoaders.find(ext) != mAudioLoaders.end())
  {	
    try
    {
      AudioStream* stream = mAudioLoaders[ext]->streamSound(filename);
      out.bind(new StreamedSound(stream, getSource()));
      mActiveSounds.push_back(out);
      if(!startPaused)
        out->play();
    }
    catch(Exception e)
    {
      std::cout<<"Could not stream sound.\n";
    }
  }
  else
  {
    std::cout<<"Could not stream sound, no loader found.\n";
  }	

  return out;
}
//-----------------------------------------------------------------------

void ALSubsystem::loadSound(String filename, String name)
{
  if(name == "AUTO")
    name = filename;

  String ext = filename.substr(filename.find_last_of(".") + 1);

  std::cout<<"Loading " << filename.substr(filename.find_last_of("/") + 1) << "... ";

  if(mAudioLoaders.find(ext) != mAudioLoaders.end())
  {
    try
    {
      ALuint buff;
      SoundData sd;
      mAudioLoaders[ext]->loadSound(filename, buff, &sd);
      mAudioData[name] = sd;
      mAudioBuffers[name] = buff;
      std::cout<<"Done\n";
    }
    catch(Exception e)
    {
      std::cout<<"Failed! "<<e.what()<<"\n";;
    }
  }
  else
  {
    std::cout<<"Failed! No audio loader found for ." << ext << " files!\n";
  }
}
//-----------------------------------------------------------------------

ALuint ALSubsystem::getSource()
{
  for(std::map<ALuint, bool>::iterator it = mSourceAvailability.begin(); 
    it != mSourceAvailability.end(); ++it)
  {
    if(it->second)
    {
      it->second = false;
      return it->first;
    }
  }
}
//-----------------------------------------------------------------------

bool ALSubsystem::hasSources()
{
  for(std::map<ALuint, bool>::iterator it = mSourceAvailability.begin(); 
    it != mSourceAvailability.end(); ++it)
    if(it->second)
      return true;
  return false;
}
//-----------------------------------------------------------------------
