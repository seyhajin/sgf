
CHECKOUT_URL=https://github.com/blitz-research/imgui.git
CHECKOUT_DIR=$PWD/ImGui

if test ! -d $CHECKOUT_DIR ; then
	echo "Checking out $CHECKOUT_URL to $CHECKOUT_DIR..."
	git clone $CHECKOUT_URL $CHECKOUT_DIR || exit 1
fi

CHECKOUT_URL="https://github.com/blitz-research/ImGuiFileDialog.git -b Lib_Only"
CHECKOUT_DIR=$PWD/ImGuiFileDialog

if test ! -d $CHECKOUT_DIR ; then
	echo "Checking out $CHECKOUT_URL to $CHECKOUT_DIR..."
	git clone $CHECKOUT_URL $CHECKOUT_DIR || exit 1
fi
