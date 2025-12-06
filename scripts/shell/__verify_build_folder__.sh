if [ ! -f "README.md" ] || [ ! -d "src" ] || [ ! -d "include" ]; then
    echo "This script was created to be called from the project root dir"
    exit 1 # Check if we are in the project root (README.md, src, include must exist)
fi

if [ ! -d "build" ]; then
    echo "Creating directory build"
    mkdir -p "build"
fi

if [ ! -d "build/tests" ]; then
    echo "Creating directory build/tests"
    mkdir -p "build/tests"
fi

if [ ! -d "build/examples" ]; then
    echo "Creating directory build/examples"
    mkdir -p "build/examples"
fi

if [ ! -d "build/out" ]; then
    echo "Creating directory build/out"
    mkdir -p "build/out"
fi

if [ ! -d "build/data" ]; then
    echo "Creating directory build/data"
    mkdir -p "build/data"
fi

echo "Build folder structure verified."
exit 0