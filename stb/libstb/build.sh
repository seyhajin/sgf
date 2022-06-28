CHECKOUT_URL=https://github.com/blitz-research/stb.git

CHECKOUT_DIR="$PWD/stb"

if test ! -f "$CHECKOUT_DIR/.okay" ; then
    echo "Checking out $CHECKOUT_URL to $CHECKOUT_DIR..."
    rm -rf "$CHECKOUT_DIR"
    git clone "$CHECKOUT_URL" "$CHECKOUT_DIR" || exit 1
    touch "$CHECKOUT_DIR/.okay"
fi

echo "done."
