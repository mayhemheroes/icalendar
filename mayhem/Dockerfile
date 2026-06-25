# syntax=docker/dockerfile:1.7
#
# Commit-image Dockerfile (SMALL form) for collective/icalendar (Python / Atheris), built ON TOP of
# the org base image (ghcr.io/mayhemheroes/base), which provides debian + clang/llvm + python3 + pip
# + the non-root identities + the build-contract ENV.
#
# mayhem/build.sh bakes a wheelhouse (online, once) — the icalendar wheel built from the in-tree
# source, all its runtime deps, and atheris — then installs from it with --no-index, so the
# air-gapped PATCH re-run (docker run --network none ... bash mayhem/build.sh) never reaches PyPI
# (SPEC §6.5). It then compiles the thin ELF launcher targets (Mayhem requires an ELF target).
#
# Build (from the repo root):
#   DOCKER_BUILDKIT=1 docker build -f mayhem/Dockerfile \
#       --build-arg COMMIT=$(git rev-parse HEAD) -t icalendar-commit:test .

FROM ghcr.io/mayhemheroes/base:20260621

ARG COMMIT
ARG REPO_URL
# SOURCE_DATE_EPOCH stays an ARG (available to build.sh); never baked as ENV — an empty value breaks
# clang. build.sh unsets it when empty.
ARG SOURCE_DATE_EPOCH
# Sanitizer override knob (declared WITHOUT a default so an unset arg lets the base ENV flow through).
ARG SANITIZER_FLAGS
# Build parallelism (optional; build.sh falls back to nproc).
ARG MAYHEM_JOBS=

# Source enters from the build CONTEXT (the CI checkout): the FULL repo (incl. .git history) is
# copied to /mayhem as `mayhem`; build.sh builds in place and installs python deps to ~/.local.
USER mayhem
WORKDIR /mayhem
COPY --chown=2000:2000 . /mayhem

RUN bash mayhem/build.sh

# Run the behavioral oracle at build time — a failing suite fails the image build.
RUN if [ -f mayhem/test.sh ]; then bash mayhem/test.sh; else echo "no mayhem/test.sh"; fi

LABEL org.opencontainers.image.source="${REPO_URL}" \
      org.opencontainers.image.revision="${COMMIT}" \
      rlenv.tier="commit" \
      rlenv.source_date_epoch="${SOURCE_DATE_EPOCH}" \
      rlenv.user="mayhem"

USER mayhem
WORKDIR /mayhem
