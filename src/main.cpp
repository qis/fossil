#include <boost/asio/version.hpp>
#include <filesystem>
#include <format>
#include <iostream>

int main(int argc, char* argv[])
{
  std::cout << std::format("{}", std::filesystem::current_path().string()) << std::endl;
  std::cout << "asio " << BOOST_ASIO_VERSION << std::endl;
}
