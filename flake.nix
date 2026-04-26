{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      package = self.packages.${system}.default;
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "audio-systray";
        version = "1.0";
        src = ./.;
        buildInputs = with pkgs; [
          cmake pkg-config gtk3 xapp.dev pulseaudio libnotify libx11 makeWrapper
          playerctl
        ];

        installPhase = ''
          mkdir -p $out/bin
          cp audio-systray $out/bin/

          mkdir -p $out/share/icons
          cp $src/icons/* $out/share/icons/

          wrapProgram $out/bin/audio-systray \
            --set GDK_PIXBUF_MODULE_FILE ${pkgs.librsvg}/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache \
            # --set ICON_PATH $out/share/icons \
            # --set PAVUCONTROL_PATH ${pkgs.pavucontrol}/bin/pavucontrol
        '';
      };

    };
}

