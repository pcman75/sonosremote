from soco import SoCo
from soco import SonosDiscovery

if __name__ == '__main__':
    sonos_devices = SonosDiscovery()

    for ip in sonos_devices.get_speaker_ips():
        device = SoCo(ip)
        zone_name = device.get_speaker_info()['zone_name']
        print "IP of %s is %s" % (zone_name, ip)
