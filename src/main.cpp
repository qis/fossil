#include <filesystem>
#include <format>
#include <iostream>

int main(int argc, char* argv[]) {
  std::cout << std::format("example: {}", std::filesystem::current_path().string()) << std::endl;
}
