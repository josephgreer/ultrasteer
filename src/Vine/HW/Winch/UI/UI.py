import serial
import Tkinter
import threading
import time

try:
    serser = serial.Serial('/dev/tty.usbserial-A603XX9M', baudrate=115200, timeout=5)  # open serial port
except serial.SerialException:
    serser = serial.Serial()
    print 'port can\'t open'


print serser.name

def read_from_port(ser):
    while True:
        if(ser.isOpen()):
            reading = ser.readline()
            print(reading)


thread = threading.Thread(target=read_from_port, args=(serser,))
thread.start()

top = Tkinter.Tk()

def executeCommand(str):
    if(serser.isOpen()):
        serser.write(str)

def executeCommandWithArgument(str,strArg):
    commandStr = str+strArg.get()
    if(serser.isOpen()):
        serser.write(commandStr)

textBoxVars = {}
textBoxVars['Increment'] = Tkinter.StringVar()
textBoxVars['Decrement'] = Tkinter.StringVar()
textBoxVars['IncPressure'] = Tkinter.StringVar()
textBoxVars['DecPressure'] = Tkinter.StringVar()
textBoxVars['Increment'].set('10')
textBoxVars['Decrement'].set('10')
textBoxVars['IncPressure'].set('0.1')
textBoxVars['DecPressure'].set('0.1')

textBoxes = {}
textBoxes['Increment'] = Tkinter.Entry(top,textvariable=textBoxVars['Increment']).grid(row=3,column=1,sticky='W')
textBoxes['Decrement'] = Tkinter.Entry(top,textvariable=textBoxVars['Decrement']).grid(row=4,column=1,sticky='W')
textBoxes['IncPressure'] = Tkinter.Entry(top,textvariable=textBoxVars['IncPressure']).grid(row=5,column=1,sticky='W')
textBoxes['DecPressure'] = Tkinter.Entry(top,textvariable=textBoxVars['IncPressure']).grid(row=6,column=1,sticky='W')

blah = Tkinter.Entry(top)
blah.setvar()

buttons = {}
buttons['PosMode'] = Tkinter.Button(top, text='Position Mode',command=lambda: executeCommand('m p\n')).grid(row=0,sticky='W')
buttons['ThrottleMode'] = Tkinter.Button(top, text='Position Mode',command=lambda: executeCommand('m t\n')).grid(row=1,sticky='W')
buttons['VelMode'] = Tkinter.Button(top, text='Velocity Mode',command=lambda: executeCommand('m v\n')).grid(row=2,sticky='W')
buttons['Increment'] = Tkinter.Button(top, text='Increment',command=lambda: executeCommandWithArgument('w ',textBoxVars['Increment'])).grid(row=3,sticky='W')
buttons['Decrement'] = Tkinter.Button(top, text='Decrement',command=lambda: executeCommandWithArgument('q ',textBoxVars['Decrement'])).grid(row=4,sticky='W')
buttons['IncPressure'] = Tkinter.Button(top, text='Increment Pressure',command=lambda: executeCommandWithArgument('l ',textBoxVars['IncPressure'])).grid(row=5,sticky='W')
buttons['DecPressure'] = Tkinter.Button(top, text='Decrement Pressure',command=lambda: executeCommandWithArgument('l ',textBoxVars['DecPressure'])).grid(row=6,sticky='W')

# for k in buttons:
#     buttons[k].pack()

top.mainloop()

serser.close()             # close port