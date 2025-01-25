class PeekTree < Formula
  desc "Enhanced directory tree viewer with file previews"
  homepage "https://github.com/aymanhki/peek_tree"
  url "https://github.com/Aymanhki/peek_tree/releases/download/v1.2.1/peek_tree-macos.tar.gz"
  sha256 "57173c10b43fc26223c154cf5cd219165a715ff27c84dc5a22dfc93cb754187e"
  license "MIT"

  def install
    bin.install "peek_tree"
  end

  test do
    system "#{bin}/peek_tree", "-h"
  end
end
