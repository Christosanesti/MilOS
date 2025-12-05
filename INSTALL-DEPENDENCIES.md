# Install Dependencies - Manual Steps

Since automated installation requires sudo password, please run these commands manually:

## Step 1: Install Dependencies

Open a terminal and run:

```bash
sudo apt-get update
sudo apt-get install -y libyaml-cpp-dev libpcap-dev
```

## Step 2: Verify Installation

```bash
pkg-config --exists yaml-cpp && echo "✅ yaml-cpp: FOUND" || echo "❌ yaml-cpp: NOT FOUND"
pkg-config --exists libpcap && echo "✅ libpcap: FOUND" || echo "❌ libpcap: NOT FOUND"
```

Both should show "FOUND" after successful installation.

## Step 3: Rebuild Test Suite

```bash
cd /home/haphaton/Desktop/MilOS/tests/build
rm -rf *
cmake -DENABLE_COVERAGE=ON ..
make -j$(nproc)
```

## Step 4: Run All Tests

```bash
cd /home/haphaton/Desktop/MilOS
./scripts/run-all-tests.sh
```

## Expected Results

After installation, you should see:
- ✅ All 7 test suites building
- ✅ 70+ tests running
- ✅ All tests passing
- ✅ Ready for coverage report

---

**Note:** If you've already installed the dependencies, skip to Step 3.

