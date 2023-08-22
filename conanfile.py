from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import get
from conan.tools.scm import Git

class TauCOMRecipe(ConanFile):
    name = "tauutils"
    package_type = "library"

    # Optional metadata
    license = "MIT"
    author = "Hyfloac mail@hyfloac.com"
    url = "https://github.com/hyfloac/TauUtils"
    description = ""

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = { "shared": [True, False] }
    default_options = { "shared": True }

    # Sources are located in the same place as this recipe, copy them to the recipe
    # exports_sources = "CMakeLists.txt", "src/*", "include/*"

    def set_version(self):
        self.version = self.conan_data["latest"];

    def source(self):
        data = self.conan_data["sources"][self.version];
        repo = self.conan_data["sources"]["repos"][data["url"]]
        git = Git(self)
        git.clone(url=repo, target=".")
        if(not ("latest" in data)):
            git.checkout(data["target"])

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if not self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        if self.options.shared:
            self.cpp_info.libs = ["tauutils-dynamic-shared"]
            self.cpp_info.set_property("cmake_target_name", "tauutils::TauUtilsDynamicShared")
        else:
            self.cpp_info.libs = ["tauutils-dynamic-static"]
            self.cpp_info.set_property("cmake_target_name", "tauutils::TauUtilsDynamicStatic")

    

    

