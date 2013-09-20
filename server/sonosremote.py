import os
import sys
import webapp2
from soco import SoCo
import json
import logging, traceback
logger = logging.getLogger(__name__)

class MainPage(webapp2.RequestHandler):

      
    sonos = SoCo('bfree.ro') # Pass in the IP of your Sonos speaker
    
    def post(self):
    
        CMD_PLAY = 1  
        command = json.loads((self.request.body))
        if command['1'] == CMD_PLAY:
            return self.play()
        else:
            return self.error()

        # Pass in a URI to a media file to have it streamed through the Sonos speaker
        #sonos.play_uri('http://archive.org/download/TenD2005-07-16.flac16/TenD2005-07-16t10Wonderboy_64kb.mp3')

        #track = sonos.get_current_track_info()

        #self.response.write(track)
        
        #queue = sonos.get_queue()
        #for item in queue:
        #    self.response.write(item['title'])
        #    self.response.write('\r\n')
        
        #sonos.pause()

        # Play a stopped or paused track
        #sonos.play()

    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.write('Sonos Remote Control')

    def play(self):
        logger.info('Play')
        self.sonos.play()
    
    def stop(self):
        self.sonos.stop()
        
    def error(self):
        self.sonos.stop()
        
def fix_path():
    # credit:  Nick Johnson of Google
    sys.path.append(os.path.join(os.path.dirname(__file__), 'requests'))
    
fix_path()
application = webapp2.WSGIApplication([('/', MainPage),], debug=True)
