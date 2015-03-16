#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

speed_t baudrate(unsigned int baud)
{
  switch (baud) {
  case 0: return B0;
  case 50: return B50;
  case 75: return B75;
  case 110: return B110;
  case 134: return B134;
  case 150: return B150;
  case 200: return B200;
  case 300: return B300;
  case 600: return B600;
  case 1200: return B1200;
  case 1800: return B1800;
  case 2400: return B2400;
  case 4800: return B4800;
  case 9600: return B9600;
  case 19200: return B19200;
  case 38400: return B38400;
  default: return B0;
  }
}

int openser(const char* path, unsigned int baud)
{
  int fd = open(path, O_RDWR|O_NOCTTY);
  if (fd < 0) return fd;
  struct termios conf;
  if (tcgetattr(fd, &conf) < 0)
    return close(fd), -1;
  cfsetispeed(&conf, baudrate(baud));
  cfsetospeed(&conf, baudrate(baud));
  cfmakeraw(&conf);
  conf.c_cc[VMIN] = 1;
  conf.c_cc[VTIME] = 0;
  if (tcsetattr(fd, TCSANOW, &conf) < 0)
    return close(fd), -1;
  return fd;
}

#include <node.h>
#include <errno.h>
#include <string.h>

#include <string>

using namespace v8;

void openser_wrap(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  std::string path(*(String::Utf8Value(args[0]->ToString())));
  const unsigned int baud = args[1]->Int32Value();
  int fd = openser(path.c_str(), baud);
  if (fd < 0) {
    isolate->ThrowException(
      Exception::Error(
        String::NewFromUtf8(isolate, strerror(errno))));
    return;
  }
  Local<Number> num = Number::New(isolate, fd);
  args.GetReturnValue().Set(num);
}

void init(Handle<Object> exports)
{
  NODE_SET_METHOD(exports, "openser", openser_wrap);
}

NODE_MODULE(openser, init)
