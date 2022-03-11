BINDIR?=bin
PREFIX?=install

CXX=g++

CXX_FLAGS=-std=c++11 -O3 -Wall -Wconversion -Wunused-function\
            -I $(PREFIX)/TestU01/include \
            -I $(PREFIX)/random123/include \
            -I $(PREFIX)/pcg-cpp/include

CXX_PEDANTIC=-pedantic

LD_FLAGS=-lm \
         $(PREFIX)/TestU01/lib/libtestu01.a \
         $(PREFIX)/TestU01/lib/libmylib.a \
         $(PREFIX)/TestU01/lib/libprobdist.a

GENERATORS=zero_generator \
           hwd_mersenne_twister \
           hwd_pcg64_ref \
           hwd_philox4x32_ref \
           hwd_xoroshiro128aox_24_16_37 \
           hwd_xoroshiro128aox_55_14_36 \
           hwd_xoroshiro128plus_24_16_37 \
           hwd_xoroshiro128plus_55_14_36 \
           mersenne_twister \
           pcg32_fast_ref \
           pcg64_fast_ref \
           pcg64_ref \
           philox2x64 \
           philox2x64_ref \
           philox4x32 \
           philox4x32_ref \
           philox4x32_ref_matchoutput \
           philox4x32_resetkeys \
           xoroshiro128aox_24_16_37 \
           xoroshiro128aox_55_14_36 \
           xoroshiro128plus_24_16_37 \
           xoroshiro128plus_55_14_36 \
           ze_mersenne_twister \
           ze_pcg64_ref \
           ze_philox4x32_ref \
           ze_xoroshiro128aox_55_14_36 \
           ze_xoroshiro128plus_55_14_36

TARGETS=$(addprefix $(BINDIR)/, $(GENERATORS))

all: outdir $(TARGETS)

.PHONY: outdir
outdir:
	mkdir -p $(BINDIR)

$(BINDIR)/zero_generator: src/main.cpp src/zero_generator.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) $^ $(LD_FLAGS) -o $@

$(BINDIR)/mersenne_twister: src/main.cpp src/mersenne_twister.cpp
	$(CXX) $(CXX_FLAGS) -DTEST_32BIT_OUTPUT $^ $(LD_FLAGS) -o $@

$(BINDIR)/pcg32_fast_ref: src/main.cpp src/pcg32_fast_ref.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) -DTEST_32BIT_OUTPUT $^ $(LD_FLAGS) -o $@

$(BINDIR)/pcg64_ref: src/main.cpp src/pcg64_ref.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) $^ $(LD_FLAGS) -o $@

$(BINDIR)/pcg64_fast_ref: src/main.cpp src/pcg64_fast_ref.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) $^ $(LD_FLAGS) -o $@

$(BINDIR)/philox4x32_ref: src/main.cpp src/philox4x32_ref.cpp
	$(CXX) $(CXX_FLAGS) $^ $(LD_FLAGS) -o $@

$(BINDIR)/philox4x32_ref_matchoutput: src/main.cpp src/philox4x32_ref.cpp
	$(CXX) $(CXX_FLAGS) $^ $(LD_FLAGS) -DMATCH_OUTPUT -o $@

$(BINDIR)/philox2x64_ref: src/main.cpp src/philox2x64_ref.cpp
	$(CXX) $(CXX_FLAGS) $^ $(LD_FLAGS) -o $@

$(BINDIR)/philox4x32: src/main.cpp src/philox.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) -DSUFFIX=4x32_10 $^ $(LD_FLAGS) -o $@

$(BINDIR)/philox4x32_resetkeys: src/main.cpp src/philox.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) -DSUFFIX=4x32_10 -DRESET_KEYS $^ $(LD_FLAGS) -o $@

$(BINDIR)/philox2x64: src/main.cpp src/philox.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) -DSUFFIX=2x64_10 $^ $(LD_FLAGS) -o $@

$(BINDIR)/xoroshiro128plus_55_14_36: src/main.cpp src/xoroshiro128.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) -DSUFFIX=plus -DSHIFT_A=55 -DSHIFT_B=14 -DSHIFT_C=36 $^ $(LD_FLAGS) -o $@

$(BINDIR)/xoroshiro128plus_24_16_37: src/main.cpp src/xoroshiro128.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) -DSUFFIX=plus -DSHIFT_A=24 -DSHIFT_B=16 -DSHIFT_C=37 $^ $(LD_FLAGS) -o $@

$(BINDIR)/xoroshiro128aox_55_14_36: src/main.cpp src/xoroshiro128.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) -DSUFFIX=aox -DSHIFT_A=55 -DSHIFT_B=14 -DSHIFT_C=36 $^ $(LD_FLAGS) -o $@

$(BINDIR)/xoroshiro128aox_24_16_37: src/main.cpp src/xoroshiro128.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) -DSUFFIX=aox -DSHIFT_A=24 -DSHIFT_B=16 -DSHIFT_C=37 $^ $(LD_FLAGS) -o $@

# HWD

$(BINDIR)/hwd_mersenne_twister: thirdparty/hwd.c thirdparty/prngs_hwd.c src/mersenne_twister.cpp
	$(CXX) $(CXX_FLAGS) -c src/mersenne_twister.cpp -o $@.o
	$(CXX) thirdparty/hwd.c $@.o -o $@

$(BINDIR)/hwd_pcg64_ref: thirdparty/hwd.c thirdparty/prngs_hwd.c src/pcg64_ref.cpp
	$(CXX) $(CXX_FLAGS) -c src/pcg64_ref.cpp -DSUFFIX=plus -o $@.o
	$(CXX) thirdparty/hwd.c $@.o -o $@

$(BINDIR)/hwd_philox4x32_ref: thirdparty/hwd.c thirdparty/prngs_hwd.c src/philox4x32_ref.cpp
	$(CXX) $(CXX_FLAGS) -c src/philox4x32_ref.cpp -o $@.o
	$(CXX) thirdparty/hwd.c $@.o -o $@

$(BINDIR)/hwd_xoroshiro128plus_55_14_36: thirdparty/hwd.c thirdparty/prngs_hwd.c src/xoroshiro128.cpp
	$(CXX) $(CXX_FLAGS) -c src/xoroshiro128.cpp -DSUFFIX=plus -DSHIFT_A=55 -DSHIFT_B=14 -DSHIFT_C=36 -o $@.o
	$(CXX) thirdparty/hwd.c $@.o -o $@

$(BINDIR)/hwd_xoroshiro128plus_24_16_37: thirdparty/hwd.c thirdparty/prngs_hwd.c src/xoroshiro128.cpp
	$(CXX) $(CXX_FLAGS) -c src/xoroshiro128.cpp -DSUFFIX=plus -DSHIFT_A=24 -DSHIFT_B=16 -DSHIFT_C=37 -o $@.o
	$(CXX) thirdparty/hwd.c $@.o -o $@

$(BINDIR)/hwd_xoroshiro128aox_55_14_36: thirdparty/hwd.c thirdparty/prngs_hwd.c src/xoroshiro128.cpp
	$(CXX) $(CXX_FLAGS) -c src/xoroshiro128.cpp -DSUFFIX=aox -DSHIFT_A=55 -DSHIFT_B=14 -DSHIFT_C=36 -o $@.o
	$(CXX) thirdparty/hwd.c $@.o -o $@

$(BINDIR)/hwd_xoroshiro128aox_24_16_37: thirdparty/hwd.c thirdparty/prngs_hwd.c src/xoroshiro128.cpp
	$(CXX) $(CXX_FLAGS) -c src/xoroshiro128.cpp -DSUFFIX=aox -DSHIFT_A=24 -DSHIFT_B=16 -DSHIFT_C=37 -o $@.o
	$(CXX) thirdparty/hwd.c $@.o -o $@

# Zero escape

$(BINDIR)/ze_mersenne_twister: src/zero-escape.cpp src/mersenne_twister.cpp
	$(CXX) $(CXX_FLAGS) -DSTATE32_OUTPUT32 $^ $(LD_FLAGS) -o $@

$(BINDIR)/ze_pcg64_ref: src/zero-escape.cpp src/pcg64_ref.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) -DSTATE128_OUTPUT64 $^ $(LD_FLAGS) -o $@

$(BINDIR)/ze_philox4x32_ref: src/zero-escape.cpp src/philox4x32_ref.cpp
	$(CXX) $(CXX_FLAGS) $^ $(LD_FLAGS) -DSTATE128_OUTPUT64 -o $@

$(BINDIR)/ze_xoroshiro128plus_55_14_36: src/zero-escape.cpp src/xoroshiro128.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) -DSTATE128_OUTPUT64 -DSUFFIX=plus -DSHIFT_A=55 -DSHIFT_B=14 -DSHIFT_C=36 $^ $(LD_FLAGS) -o $@

$(BINDIR)/ze_xoroshiro128aox_55_14_36: src/zero-escape.cpp src/xoroshiro128.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_PEDANTIC) -DSTATE128_OUTPUT64 -DSUFFIX=aox -DSHIFT_A=55 -DSHIFT_B=14 -DSHIFT_C=36 $^ $(LD_FLAGS) -o $@


test:
	@./$(BINDIR)/pcg32_fast_ref test x 1 0
	@./$(BINDIR)/mersenne_twister test x 1 0
	@./$(BINDIR)/pcg64_ref test x 1 0
	@./$(BINDIR)/pcg64_fast_ref test x 1 0
	@./$(BINDIR)/philox4x32_ref test x 1 0
	@./$(BINDIR)/philox2x64_ref test x 1 0
	@./$(BINDIR)/philox4x32 test x 1 0
	@./$(BINDIR)/philox2x64 test x 1 0
	@./$(BINDIR)/xoroshiro128plus_55_14_36 test x 1 0
	@./$(BINDIR)/xoroshiro128plus_24_16_37 test x 1 0
	@./$(BINDIR)/xoroshiro128aox_55_14_36 test x 1 0
	@./$(BINDIR)/xoroshiro128aox_24_16_37 test x 1 0
	@# Check the Philox implementation matches the reference.
	@echo "Checking Philox4x32 implementation"
	@./$(BINDIR)/philox4x32_resetkeys stdout std32 0 0 | hexdump -Cv | head > $(BINDIR)/philox4x32-impl.output
	@./$(BINDIR)/philox4x32_ref_matchoutput stdout std32 0 0 | hexdump -Cv | head > $(BINDIR)/philox4x32-ref.output
	@diff $(BINDIR)/philox4x32-impl.output $(BINDIR)/philox4x32-ref.output || echo "Philox4x32 implemenation does not match reference"

.PHONY: clean
clean:
	rm -rf $(BINDIR)
