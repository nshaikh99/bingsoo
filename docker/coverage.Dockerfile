# Define builder stage
FROM bingsoo:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure

# Build and generate coverage report
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage
