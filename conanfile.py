from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import get
from conan.tools.scm import Git
from conan.tools.files import copy

class TauCOMRecipe(ConanFile):
    name = "tauutils"
    package_type = "library"

    # Optional metadata
    license = "MIT"
    author = "Hyfloac <mail@hyfloac.com>"
    url = "https://github.com/hyfloac/TauUtils"
    description = ""

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "genTest": [True, False]
    }
    default_options = {
        "shared": True,
        "genTest": False
    }

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "TauUtilsDynamic/*", "cmake/*"

    def set_version(self):
        self.version = self.conan_data["latest"]

    # def source(self):
    #     data = self.conan_data["sources"][self.version];
    #     repo = self.conan_data["sources"]["repos"][data["url"]]
    #     if("latest" in data):
    #         copy(self, "CMakeLists.txt", self.recipe_folder, self.export_sources_folder)
    #         self.exports_sources = "CMakeLists.txt", "TauUtilsDynamic/*"
    #     else:
    #         git = Git(self)
    #         git.clone(url=repo, target=".")
    #         git.checkout(data["target"])

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
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.variables["GEN_TEST"] = self.options.genTest
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["TauUtils"]
