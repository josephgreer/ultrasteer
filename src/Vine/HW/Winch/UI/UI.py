import serial
import Tkinter
import threading
import time

runThread = True
doneThread = False

try:
    serser = serial.Serial('/dev/tty.usbserial-A603XX9M', baudrate=115200, timeout=5)  # open serial port
except serial.SerialException:
    serser = serial.Serial()
    print 'port can\'t open'


print serser.name

def read_from_port(ser):
    while runThread:
        if(ser.isOpen()):
            reading = serser.readline()
            txt.delete(Tkinter.CURRENT,Tkinter.END)
            txt.insert(Tkinter.CURRENT,reading)

    doneThread = True


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
textBoxes['Increment'] = Tkinter.Entry(top,width=5,textvariable=textBoxVars['Increment']).grid(row=3,column=1,sticky=Tkinter.W)
textBoxes['Decrement'] = Tkinter.Entry(top,width=5,textvariable=textBoxVars['Decrement']).grid(row=4,column=1,sticky=Tkinter.W)
textBoxes['IncPressure'] = Tkinter.Entry(top,width=5,textvariable=textBoxVars['IncPressure']).grid(row=5,column=1,sticky=Tkinter.W)
textBoxes['DecPressure'] = Tkinter.Entry(top,width=5,textvariable=textBoxVars['IncPressure']).grid(row=6,column=1,sticky=Tkinter.W)

buttons = {}
buttons['PosMode'] = Tkinter.Button(top, text='Position Mode',command=lambda: executeCommand('m p\n')).grid(row=0,sticky='W')
buttons['ThrottleMode'] = Tkinter.Button(top, text='Throttle Mode',command=lambda: executeCommand('m t\n')).grid(row=1,sticky='W')
buttons['VelMode'] = Tkinter.Button(top, text='Velocity Mode',command=lambda: executeCommand('m v\n')).grid(row=2,sticky='W')
buttons['Increment'] = Tkinter.Button(top, text='Increment',command=lambda: executeCommandWithArgument('w ',textBoxVars['Increment'])).grid(row=3,sticky='W')
buttons['Decrement'] = Tkinter.Button(top, text='Decrement',command=lambda: executeCommandWithArgument('q ',textBoxVars['Decrement'])).grid(row=4,sticky='W')
buttons['IncPressure'] = Tkinter.Button(top, text='Increment Pressure',command=lambda: executeCommandWithArgument('l ',textBoxVars['IncPressure'])).grid(row=5,sticky='W')
buttons['DecPressure'] = Tkinter.Button(top, text='Decrement Pressure',command=lambda: executeCommandWithArgument('l ',textBoxVars['DecPressure'])).grid(row=6,sticky='W')

# create text widget
txt = Tkinter.Text(top, borderwidth=3, relief="sunken", width=60,height=5)
txt.config(font=("consolas", 12), undo=True, wrap='word')
txt.grid(row=7, column=0, columnspan=2, sticky="nsew", padx=2, pady=2)

# create a Scrollbar and associate it with txt
scrollb = Tkinter.Scrollbar(top, command=txt.yview)
scrollb.grid(row=7, column=3, sticky='nsew')
txt['yscrollcommand'] = scrollb.set
# txt['state'] = Tkinter.DISABLED

# for k in buttons:
#     buttons[k].pack()

top.mainloop()

runThread = False
while not doneThread:
    time.sleep(0.1)

serser.close()             # close port