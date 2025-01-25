class PeekTree < Formula
  desc "Enhanced directory tree viewer with file previews"
  homepage "https://github.com/aymanhki/peek_tree"
  url "https://github.com/Aymanhki/peek_tree/releases/download/v1.1.9/peek_tree-macos.tar.gz"
  sha256 "92a086dd72699f3c1a8fd8d00fe0e5b531504545ff3efd1410511d08ac0f68e6"
  license "MIT"

  def install
    bin.install "peek_tree"
  end

  test do
    system "#{bin}/peek_tree", "-h"
  end
end