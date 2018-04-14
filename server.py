from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
from SocketServer import ThreadingMixIn
import threading
import subprocess

colorstring = "0+000000,255+FFFFFF"

class Handler(BaseHTTPRequestHandler):
    def do_STATIC(self, filename, mime):
        f = open("static/" + filename)
        self.send_response(200)
        self.send_header('Content-type', mime)
        self.send_header("Cache-Control", "public, max-age=3600")
        self.end_headers()
        self.wfile.write(f.read())
        f.close()

    # This works for everything except Julia sets.
    def do_GENERIC_FRACTAL(self, kernel, x_min):
        global colorstring
        tile_descriptor_string = self.path.split("/")[-1]
        (x, y, z) = [t(s) for t,s in zip((int,int,int), tile_descriptor_string.split("."))]
        width = 4.0/pow(2, min(z, 36))
        x = x_min + x*width
        y = -2 + y*width
        command = "./kernels/{0} {1} {2} {3} | ./kernels/color {4} | convert - jpg:-".format(kernel, x,y,width, colorstring)
        image = subprocess.check_output(command, shell=True)
        self.send_response(200)
        self.send_header('Content-type', "image/jpg")
        self.send_header("Cache-Control", "public, max-age=0")
        self.end_headers()
        self.wfile.write(image)

    def do_GET(self):
        global colorstring
        if self.path == "/":
            self.do_STATIC("index.html", "text/html")
        elif self.path.startswith("/setcolor/"):
            colorstring = self.path.split("/")[-1]
            self.send_response(200)
            self.end_headers()
            self.wfile.write(colorstring)
        elif self.path.startswith("/julia/") and not self.path.endswith(".jpg"):
            self.do_STATIC("julia.html", "text/html")
        elif self.path == "/burningship" or self.path == "/burningship/":
            self.do_STATIC("burningship.html", "text/html")
        elif self.path == "/mandelbrot" or self.path == "/mandelbrot/":
            self.do_STATIC("mandelbrot.html", "text/html")
        elif self.path == "/collatz" or self.path == "/collatz/":
            self.do_STATIC("collatz.html", "text/html")
        elif self.path == "/ol.js":
            self.do_STATIC("ol-4.1.1/ol.js", "application/javascript; charset=utf-8")
        elif self.path == "/ol.css":
            self.do_STATIC("ol-4.1.1/ol.css", "text/css")
        elif self.path.startswith("/julia/"):
            # Julia sets are a special case because the constant is also encoded in the URL.
            split = self.path.split("/")
            tile_descriptor_string1 = split[2]
            tile_descriptor_string2 = split[-1]
            (cr, ci) = [t(s) for t,s in zip((float,float), tile_descriptor_string1.split(","))]
            (x, y, z) = [t(s) for t,s in zip((int,int,int), tile_descriptor_string2.split("."))]
            width = 4.0/pow(2, min(z, 36))
            x = -2 + x*width
            y = -2 + y*width
            command = "./kernels/julia {0} {1} {2} {3} {4} | ./kernels/color {5} | convert - jpg:-".format(cr,ci,x,y,width, colorstring)
            image = subprocess.check_output(command, shell=True)
            self.send_response(200)
            self.send_header('Content-type', "image/jpg")
            self.send_header("Cache-Control", "public, max-age=0")
            self.end_headers()
            self.wfile.write(image)
        elif self.path.startswith("/burningship/"):
            self.do_GENERIC_FRACTAL("burningship", -2)
        elif self.path.startswith("/mandelbrot/"):
            self.do_GENERIC_FRACTAL("mandelbrot", -2)
        elif self.path.startswith("/collatz/"):
            self.do_GENERIC_FRACTAL("collatz", -4)
        else:
            self.send_response(404)
            self.send_header('Content-type', "text/html")
            self.end_headers()
            self.wfile.write("<h1>404 - Not Found</h1>\n")


class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    """"""

if __name__ == '__main__':
    server = ThreadedHTTPServer(('', 8080), Handler)
    server.serve_forever()