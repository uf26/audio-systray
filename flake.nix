{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
      nixosLib = import "${nixpkgs}/nixos/lib";
      mkOption = nixosLib.mkOption;
      types = nixosLib.types;
      mkIf = nixosLib.mkIf;

      audioSystrayPkg = pkgs.stdenv.mkDerivation {
        pname = "audio-systray";
        version = "1.0";
        src = ./.;
        buildInputs = with pkgs; [
          cmake pkg-config gtk3 xapp.dev pulseaudio libnotify xorg.libX11
        ];
        installPhase = ''
          make
          mkdir -p $out/bin
          cp audio_systray $out/bin/
        '';
      };
    in {
      packages.${system}.default = audioSystrayPkg;

      nixosModules.default = { config, pkgs, ... }: {
        options = {
          audioSystray = {
            enable = mkOption {
              type = types.bool;
              default = false;
              description = "Enable audio-systray service.";
            };
          };
        };

        config = mkIf config.audioSystray.enable {
          environment.systemPackages = [ audioSystrayPkg ];

          systemd.services.audioSystray = {
            description = "Audio Systray Service";
            wantedBy = [ "multi-user.target" ];
            serviceConfig = {
              ExecStart = "${audioSystrayPkg}/bin/audio_systray";
              Restart = "always";
              RestartSec = 5;
            };
          };
        };
      };
    };
}



