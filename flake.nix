{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "audio-systray";
        version = "1.0";

        src = ./.;

        buildInputs = with pkgs; [
          cmake
          pkg-config
          gtk3
          xapp.dev
          pulseaudio
          libnotify
          xorg.libX11
        ];

        installPhase = ''
          make
          mkdir -p $out/bin
          cp audio_systray $out/bin/
        '';
      };
    };
}

