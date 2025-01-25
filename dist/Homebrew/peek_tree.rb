class PeekTree < Formula
  desc "Enhanced directory tree viewer with file previews"
  homepage "https://github.com/aymanhki/peek_tree"
  url "https://github.com/Aymanhki/peek_tree/releases/download/v1.2.2/peek_tree-macos.tar.gz"
  sha256 "d0515a4c2aa73f72f4355973a20fe7eddb0b87ee13ab001b78efba41c18c2350"
  license "MIT"

  def install
    bin.install "peek_tree"
  end

  test do
    system "#{bin}/peek_tree", "-h"
  end
end
