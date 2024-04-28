#!/bin/bash

getRowVal() {
        row="$(echo "$1" | head -n "$2" | tail -n 1)"
        echo "$row" | cut -d ':' -f 2 | tr -d ' '
}

getPhy() {
        echo "$(getRowVal "$1" 2)"
}

getVlans() {
        echo "$(getRowVal "$1" 3)"
}

getHosts() {
        echo "$(getRowVal "$1" 4)"
}

getFailover() {
        echo "$(getRowVal "$1" 5)"
}

getVPN() {
        echo "$(getRowVal "$1" 6)"
}

getPeers() {
        echo "$(getRowVal "$1" 7)"
}

getVLAN() {
        echo "$(getRowVal "$1" 8)"
}

getLicense() {
        row="$(echo "$1" | head -n 9 | tail -n 1)"
        echo "$row" | sed -E 's/This platform has a[n]{0,1} //' | sed -E 's/ license.//'
}

getSN() {
        echo "$(getRowVal "$1" 10)"
}

getKey() {
        echo "$(getRowVal "$1" 11)"
}

[[ $# == 2 ]] || { echo 'script should take 2 args' >&2; exit 1; }

[[ -d "$2" ]] || { echo '2nd args should be a dir' >&2; exit 2; }

touch "$1"

echo 'hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLANTrunkPorts,license,SN,key' > "$1"

hostname=""
phy=""
vlans=""
hosts=""
failover=""
VPN=""
peers=""
VLAN=""
license=""
SN=""
key=""

while read file; do
        hostname="$(basename "$file" | cut -d '.' -f 1)"

        content="$(cat "$file")"

        phy="$(getPhy "$content")"
        vlans="$(getVlans "$content")"
        hosts="$(getHosts "$content")"
        failover="$(getFailover "$content")"
        VPN="$(getVPN "$content")"
        peers="$(getPeers "$content")"
        VLAN="$(getVLAN "$content")"
        license="$(getLicense "$content")"
        SN="$(getSN "$content")"
        key="$(getKey "$content")"

        echo "${hostname},${phy},${vlans},${hosts},${failover},${VPN},${peers},${VLAN},${license},${SN},${key}" >> "$1"

done < <(find "$2" -type f -name "*.log")
