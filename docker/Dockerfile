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

# TODO(!): Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/conf/sample.conf .
COPY --from=builder /usr/src/project/static ./static
COPY --from=builder /usr/src/project/markdown ./markdown
COPY --from=builder /usr/src/project/README.md .

# TODO(!): Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# TODO(!): Update with real server name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# TODO(!): Update with real server args
CMD ["sample.conf"]
