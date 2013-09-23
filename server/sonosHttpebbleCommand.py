from soco import SoCo
import json
import logging, traceback
logger = logging.getLogger(__name__)

class SonosHttpebbleCommand:

    CMD_PLAY = 1
    CMD_STOP = 2
    CMD_PAUSE = 3
    CMD_PREV = 4
    CMD_NEXT = 5
    CMD_INFO = 6
    
    TRACK_INFO_ARTIST = 3
    TRACK_INFO_ALBUM = 4
    TRACK_INFO_TITLE = 5
    TRACK_INFO_POSITION = 6
    TRACK_INFO_ALBUM_ART = 7
    
    sonos = SoCo('bfree.ro') # Pass in the IP of your Sonos speaker
    
    def callCommand(self, commandString):
        
        command = json.loads(commandString)
        
        if command['1'] == self.CMD_PLAY:
            commandResult = self.play()
        elif command['1'] == self.CMD_STOP:
            commandResult = self.stop()
        elif command['1'] == self.CMD_PAUSE:
            commandResult = self.pause() 
        elif command['1'] == self.CMD_PREV:
            commandResult = self.previous()                    
        elif command['1'] == self.CMD_NEXT:
            commandResult = self.next() 
        elif command['1'] == self.CMD_INFO:
            commandResult = self.getCurrentTrackInfo() 
        else:
            commandResult = False;
        
        if type(commandResult) == bool:
            httpeble_response = self.create_httpeble_response(commandResult, command['1'])
        else:
            httpeble_response = self.create_httpeble_response(commandResult.pop('success'), command['1'])
            httpeble_response.update(commandResult)
            
        return httpeble_response
            
    def create_httpeble_response(self, commandResult, command):
        if(commandResult == True):
            commandResult = 1
        else:
            commandResult = 0           
        return {"1": command, "2": ["b", commandResult]}
    
    def play(self):
        return self.sonos.play()
    
    def stop(self):
        return self.sonos.stop()

    def pause(self):
        return self.sonos.pause()
        
    def previous(self):
        return self.sonos.previous()
        
    def next(self):
        return self.sonos.next()
        
    def error(self):
        return self.sonos.stop()
    
    def getCurrentTrackInfo(self):
        track = self.sonos.get_current_track_info()
        trackInfo = {}
        trackInfo[str(self.TRACK_INFO_TITLE)] = track['title']
        trackInfo[str(self.TRACK_INFO_ARTIST)] = track['artist']
        trackInfo[str(self.TRACK_INFO_ALBUM)] = track['album']
        trackInfo[str(self.TRACK_INFO_POSITION)] = track['position']
        trackInfo[str(self.TRACK_INFO_ALBUM_ART)] = track['album_art']
        trackInfo['success'] = (track['title'] != '')
        return trackInfo            
        
if __name__ == "__main__":
    test = SonosHttpebbleCommand()
    print test.callCommand('{"1":1}')
    #print test.callCommand('{"1":2}')
    print test.callCommand('{"1":6}')