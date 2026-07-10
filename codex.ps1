$env:Path = "D:\tool\codex_cli\npm-global;$env:Path"
$env:CODEX_HOME="D:\tool\codex_cli\home"
$env:RUST_LOG="error"
codex --sandbox workspace-write --ask-for-approval on-request