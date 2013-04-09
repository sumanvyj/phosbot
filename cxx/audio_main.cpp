#include <boost/python.hpp>

void initialize() {
  std::cout<<"Initializing audio system...\n";
  // TODO
  std::cout<<"Audio initialized.\n";
}

void deinitialize() {
  std::cout<<"Deinitializing audio system...\n";
  // TODO
  std::cout<<"Audio deinitialized.\n";
}

BOOST_PYTHON_MODULE(audio) {
    using namespace boost::python;
    def("initialize", initialize);
    def("deinitialize", deinitialize);
}
