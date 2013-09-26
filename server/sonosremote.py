import os
import sys
import webapp2
import logging, traceback
from sonosHttpebbleCommand import SonosHttpebbleCommand


logger = logging.getLogger(__name__)

class MainPage(webapp2.RequestHandler):

    def post(self):
    
        command = self.request.body
        httpebbleCommand = SonosHttpebbleCommand()
        response = httpebbleCommand.callCommand(command)
        
        self.response.headers['Content-Type'] = 'application/json'
        #self.response.headers['Expires'] = '0'
        return self.response.write(response)       
   
    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.write('Sonos Remote Control')
        
def fix_path():
    # credit:  Nick Johnson of Google
    sys.path.append(os.path.join(os.path.dirname(__file__), 'requests'))
    
fix_path()
application = webapp2.WSGIApplication([(r'.*', MainPage),], debug=True)


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

