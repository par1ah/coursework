#!/usr/bin/env python3
from http.server import BaseHTTPRequestHandler, HTTPServer
import logging
import urllib
import pymongo

client = pymongo.MongoClient("mongodb+srv://root:root@cluster0-elszs.mongodb.net/test?retryWrites=true&w=majority")
db = client.C_game
coll = db.Best_reuslt

class S(BaseHTTPRequestHandler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        logging.info("GET request,\nPath: %s\nHeaders:\n%s\n", str(self.path), str(self.headers))
        self._set_response()
        self.wfile.write("GET request for {}".format(self.path).encode('utf-8'))

        new_res = (self.path)[7:]
        old_res = coll.find({'_id': "5eca943b08b00a2833c528f9"}).distinct('Best_score')[0]
        print(old_res)
        if int(new_res) > int(old_res):
            coll.update_one({"_id": '5eca943b08b00a2833c528f9'}, {"$set": {"Best_score": str(new_res)}}, upsert=False)


    def do_POST(self):
        content_length = int(self.headers['Content-Length'])  # <--- Gets the size of data
        post_data = self.rfile.read(content_length)  # <--- Gets the data itself
        logging.info("POST request,\nPath: %s\nHeaders:\n%s\n\nBody:\n%s\n",
                     str(self.path), str(self.headers), post_data.decode('utf-8'))

        self._set_response()
        self.wfile.write("POST request for {}".format(self.path).encode('utf-8'))
        length = int(self.headers['Content-Length'])
        post_data = urllib.parse.parse_qs(self.rfile.read(length).decode('utf-8'))
        print(post_data)


def run(server_class=HTTPServer, handler_class=S, port=8080):
    logging.basicConfig(level=logging.INFO)
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    logging.info('Starting httpd...\n')
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    logging.info('Stopping httpd...\n')


if __name__ == '__main__':
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()