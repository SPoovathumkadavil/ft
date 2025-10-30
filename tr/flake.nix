{
  description = "c++ flake for timed_runner";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            gnumake # for 'make'
            clang-tools # for clangd
            clang # or gcc
            gdb # for debugging
            bear # for compile_db
          ];

          shellHook = ''
            echo "entering c++ development shell for timed_runner..."
          '';
        };
      });
}
