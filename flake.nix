{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    home-manager.url = "github:nix-community/home-manager";
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
          cmake pkg-config gtk3 xapp.dev pulseaudio libnotify xorg.libX11 makeWrapper
        ];

        installPhase = ''
          mkdir -p $out/bin
          cp audio-systray $out/bin/

          mkdir -p $out/share/icons
          cp $src/icons/* $out/share/icons/

          wrapProgram $out/bin/audio-systray \
            --prefix PATH : ${pkgs.pavucontrol}/bin \
            --prefix PATH : ${pkgs.playerctl}/bin \
            --set GDK_PIXBUF_MODULE_FILE ${pkgs.librsvg}/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache \
            --set ICON_PATH $out/share/icons
        '';
      };
    };
}

