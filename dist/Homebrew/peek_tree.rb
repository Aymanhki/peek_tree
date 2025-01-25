class PeekTree < Formula
  desc "Enhanced directory tree viewer with file previews"
  homepage "https://github.com/aymanhki/peek_tree"
  url "https://github.com/aymanhki/peek_tree/releases/download/v1/peek_tree-macos.tar.gz"
  sha256 "REPLACE_WITH_ACTUAL_SHA256"
  license "MIT"

  def install
    bin.install "peek_tree"
  end

  test do
    system "#{bin}/peek_tree", "-h"
  end
end