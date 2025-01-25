class PeekTree < Formula
  desc "Enhanced directory tree viewer with file previews"
  homepage "https://github.com/aymanhki/peek_tree"
  url "https://github.com/Aymanhki/peek_tree/releases/download/v1.1.7/peek_tree-macos.tar.gz"
  sha256 "cfd2e94bb45dbdf55d35ac3d1e57d6f9e7db3ac8f4dcc49a0111f4e2cf426e30"
  license "MIT"

  def install
    bin.install "peek_tree"
  end

  test do
    system "#{bin}/peek_tree", "-h"
  end
end