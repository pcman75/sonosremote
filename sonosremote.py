import os
import sys
import webapp2
from soco import SoCo


class MainPage(webapp2.RequestHandler):

    def post(self):
        #self.response.write(self.request.body)
        sonos = SoCo('bfree.ro') # Pass in the IP of your Sonos speaker

        # Pass in a URI to a media file to have it streamed through the Sonos speaker
        #sonos.play_uri('http://archive.org/download/TenD2005-07-16.flac16/TenD2005-07-16t10Wonderboy_64kb.mp3')

        track = sonos.get_current_track_info()

        self.response.write(track['title'])

        sonos.pause()

        # Play a stopped or paused track
        sonos.play()

    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.write('Sonos Remote Control')


def fix_path():
    # credit:  Nick Johnson of Google
    sys.path.append(os.path.join(os.path.dirname(__file__), 'requests'))
    
fix_path()
application = webapp2.WSGIApplication([('/', MainPage),], debug=True)
