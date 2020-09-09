C:\MinGW\share\gdb\python\gdb

https://stackoverflow.com/questions/395169/using-cmake-to-generate-visual-studio-c-project-files

to get the submodules run:

```bash
	git submodule update --init --recursive
```

to update submodules run

```bash
	git pull --recurse-submodules
```

# Tried to install Abseil but was painful to include

https://abseil.io/docs/cpp/quickstart-cmake
https://github.com/abseil/abseil-cpp.git

https://docs.bazel.build/versions/master/install-os-x.html
Install the Bazel package via Homebrew as follows:

brew tap bazelbuild/tap
brew install bazelbuild/tap/bazel
All set! You can confirm Bazel is installed successfully by running the following command:

bazel --version
Once installed, you can upgrade to a newer version of Bazel using the following command:

brew upgrade bazelbuild/tap/bazel

git submodule add https://github.com/fmtlib/fmt.git ./submodules/fmt
git submodule add https://github.com/gabime/spdlog.git ./submodules/spdlog
git submodule add https://github.com/Tencent/rapidjson.git ./submodules/rapidjson
