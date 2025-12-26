#!/bin/bash
# Create release packages for SNEJK2D

set -e

VERSION=${1:-dev}

echo "Creating release packages for version: $VERSION"

# Clean previous builds
echo "Cleaning previous builds..."
make clean
rm -f *.tar.gz *.zip
rm -f snejk2d snejk2d.exe *.dll

# Build Linux version
echo ""
echo "Building Linux version..."
make

# Create Linux release directory
mkdir -p release_tmp/snejk2d-$VERSION-linux
cp snejk2d release_tmp/snejk2d-$VERSION-linux/
cp -r assets release_tmp/snejk2d-$VERSION-linux/
cp README.md release_tmp/snejk2d-$VERSION-linux/
cp LICENSE release_tmp/snejk2d-$VERSION-linux/

# Package Linux version
echo "Packaging Linux version..."
cd release_tmp
tar -czf ../snejk2d-$VERSION-linux.tar.gz snejk2d-$VERSION-linux/
cd ..
rm -rf release_tmp

echo "✓ Linux package created: snejk2d-$VERSION-linux.tar.gz"

# Build Windows version
echo ""
echo "Building Windows version..."
./build_windows.sh

# Create Windows release directory
mkdir -p release_tmp/snejk2d-$VERSION-windows
cp snejk2d.exe release_tmp/snejk2d-$VERSION-windows/
cp *.dll release_tmp/snejk2d-$VERSION-windows/
cp -r assets release_tmp/snejk2d-$VERSION-windows/
cp README.md release_tmp/snejk2d-$VERSION-windows/
cp LICENSE release_tmp/snejk2d-$VERSION-windows/

# Package Windows version
echo "Packaging Windows version..."
cd release_tmp
zip -r ../snejk2d-$VERSION-windows.zip snejk2d-$VERSION-windows/
cd ..
rm -rf release_tmp

echo "✓ Windows package created: snejk2d-$VERSION-windows.zip"

# Clean up build artifacts
echo ""
echo "Cleaning up..."
rm -f snejk2d snejk2d.exe *.dll

echo ""
echo "✓ Release packages created successfully!"
echo ""
echo "Files created:"
echo "  - snejk2d-$VERSION-linux.tar.gz"
echo "  - snejk2d-$VERSION-windows.zip"
echo ""
echo "To create a GitHub release:"
echo "  gh release create $VERSION snejk2d-$VERSION-linux.tar.gz snejk2d-$VERSION-windows.zip"
