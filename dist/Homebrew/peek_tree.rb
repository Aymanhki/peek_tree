class PeekTree < Formula
  desc "Enhanced directory tree viewer with file previews"
  homepage "https://github.com/aymanhki/peek_tree"
  url "https://github.com/Aymanhki/peek_tree/releases/download/v1.1.8/peek_tree-macos.tar.gz"
  sha256 "7ea80d7dd97ac0f9c52f82121a8192338dc824207717a983ccfff7528a06d02a"
  license "MIT"

  def install
    bin.install "peek_tree"
  end

  test do
    system "#{bin}/peek_tree", "-h"
  end
end
