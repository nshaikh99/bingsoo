### Build/test container ###
# Define builder stage
FROM bingsoo:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure

### Deploy container ###
# Define deploy stage
FROM ubuntu:jammy as deploy

# Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/conf/sample.conf .
COPY --from=builder /usr/src/project/static ./static

# Expose some port(s)
EXPOSE 80

# Specify the binary name
ENTRYPOINT ["./server"]

# Specify arguments to ENTRYPOINT
CMD ["sample.conf"]
