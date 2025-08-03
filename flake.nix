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
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "audio-systray";
        version = "1.0";
        src = ./.;
        buildInputs = with pkgs; [
          cmake pkg-config gtk3 xapp.dev pulseaudio libnotify xorg.libX11 makeWrapper
        ];

        postInstall = ''
          wrapProgram $out/bin/audio-systray \
            --prefix PATH : ${pkgs.pavucontrol}/bin \
            --prefix PATH : ${pkgs.playerctl}/bin \
        '';
      };

      homeManagerModules = {
        default = { config, lib, ... }: {
          options = {
            services.audio-systray.enable = lib.mkEnableOption "Enable audio-systray user service";
          };

          config = lib.mkIf config.services.audio-systray.enable {
            systemd.user.services.audio-systray = {
              Unit = {
                Description = "audio systray";
                Requires = [ "tray.target" ];
                After = [
                  "graphical-session.target"
                    "tray.target"
                ];
                PartOf = [ "graphical-session.target" ];
              };

              Install = {
                WantedBy = [ "graphical-session.target" ];
              };

              Service = {
                ExecStart = "${self.packages.${system}.default}/bin/audio-systray"; 
                Restart = "on-failure";
              };
            };
          };
        };
      };

    };
}

