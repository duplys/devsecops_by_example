FROM ubuntu:20.04

# Install prerequisites
RUN apt update
RUN apt install apt-transport-https curl gnupg -y

# Install Bazel
RUN curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor >bazel-archive-keyring.gpg
RUN mv bazel-archive-keyring.gpg /usr/share/keyrings
RUN echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list
RUN apt update && apt install bazel -y

# Install clang
RUN apt install clang -y

# Create directory for working with FuzzTest
WORKDIR /opt/fuzztest