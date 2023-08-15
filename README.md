# bmd-plugin-avahi
Bmd plugin avahi publishes VM's vnc port using `avahi-publish` command.
Clients can lookup the vnc port by mDNS protocol. If you use MacOS,
open Finder and Network placement folder then you can find the VM name. And
click it to connect to the remove vnc display. Gtk client `bvnc` is also
available in 'avahi-gtk3' package.

This plugin doesn't have any configurations. Just install and restart the bmd,
this plugin starts to work.

## Requirements

This plugin requires following ports/packages.

 - avahi-app
