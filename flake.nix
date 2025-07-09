{
  description = "Development environment for audio systray";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs, ... }: {
    devShells.x86_64-linux.default =
      let
        pkgs = nixpkgs.legacyPackages.x86_64-linux;
      in
      pkgs.mkShell { 
        buildInputs = with pkgs; [
          cmake
          gnumake
          gcc
          pkg-config

          gtk3
          xapp.dev
          pulseaudio
          libnotify
          xorg.libX11
        ];
      };
  };
}
