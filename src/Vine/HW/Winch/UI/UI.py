import serial
import Tkinter
import threading
import time

runThread = True
doneThread = False

currentAct = 0
turnNoTurns = {0: 0, 1: 1}
turnLabels = ['not turning', 'turning']

try:
    serser = serial.Serial('/dev/tty.usbserial-A603XX9M', baudrate=115200, timeout=5)  # open serial port
except serial.SerialException:
    serser = serial.Serial()
    print 'port can\'t open'


print serser.name

def read_from_port(ser):
    time.sleep(0.5)
    while runThread:
        if(ser.isOpen()):
            reading = serser.readline().strip()
            # txt.delete("1.0",Tkinter.END)
            txt.insert(Tkinter.CURRENT,reading+'\n')

            readPart = reading.split(' ')
            if(readPart[0] == 'act'):
                currentAct = int(readPart[1])-1
                currentAct = min(currentAct,len(turnNoTurns.keys())-1)
                textBoxVars['ActuatorIdx'].set(str(currentAct+1))
                textBoxVars['TurnNoTurn'].set(turnLabels[currentAct])
                if(turnNoTurns[currentAct] == 0):
                    ser.write('t n')
                else:
                    ser.write('t y')

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
textBoxVars['ActuatorIdx'] = Tkinter.StringVar()
textBoxVars['TurnNoTurn'] = Tkinter.StringVar()

textBoxVars['Increment'].set('10')
textBoxVars['Decrement'].set('10')
textBoxVars['IncPressure'].set('0.6')
textBoxVars['DecPressure'].set('0.6')
textBoxVars['ActuatorIdx'].set('1')
textBoxVars['TurnNoTurn'].set(turnLabels[turnNoTurns[currentAct]])

textBoxes = {}
textBoxes['Increment'] = Tkinter.Entry(top,width=5,textvariable=textBoxVars['Increment']).grid(row=3,column=1,sticky=Tkinter.W)
textBoxes['Decrement'] = Tkinter.Entry(top,width=5,textvariable=textBoxVars['Decrement']).grid(row=4,column=1,sticky=Tkinter.W)
textBoxes['IncPressure'] = Tkinter.Entry(top,width=5,textvariable=textBoxVars['IncPressure']).grid(row=5,column=1,sticky=Tkinter.W)
textBoxes['DecPressure'] = Tkinter.Entry(top,width=5,textvariable=textBoxVars['DecPressure']).grid(row=6,column=1,sticky=Tkinter.W)
textBoxes['ActuatorIdx'] = Tkinter.Entry(top,width=5,textvariable=textBoxVars['ActuatorIdx']).grid(row=11,column=1,sticky=Tkinter.W)
textBoxes['TurnNoTurn'] = Tkinter.Entry(top, width=10, textvariable=textBoxVars['TurnNoTurn']).grid(row=12,column=1,sticky=Tkinter.W)

buttons = {}
buttons['PosMode'] = Tkinter.Button(top, text='Position Mode',command=lambda: executeCommand('m p\n')).grid(row=0,sticky=Tkinter.E)
buttons['ThrottleMode'] = Tkinter.Button(top, text='Throttle Mode',command=lambda: executeCommand('m t\n')).grid(row=1,sticky=Tkinter.E)
buttons['VelMode'] = Tkinter.Button(top, text='Velocity Mode',command=lambda: executeCommand('m v\n')).grid(row=2,sticky=Tkinter.E)
buttons['Increment'] = Tkinter.Button(top, text='Increment',command=lambda: executeCommandWithArgument('w ',textBoxVars['Increment'])).grid(row=3,sticky=Tkinter.E)
buttons['Decrement'] = Tkinter.Button(top, text='Decrement',command=lambda: executeCommandWithArgument('q ',textBoxVars['Decrement'])).grid(row=4,sticky=Tkinter.E)
buttons['IncPressure'] = Tkinter.Button(top, text='Increment Pressure',command=lambda: executeCommandWithArgument('l i ',textBoxVars['IncPressure'])).grid(row=5,sticky=Tkinter.E)
buttons['DecPressure'] = Tkinter.Button(top, text='Decrement Pressure',command=lambda: executeCommandWithArgument('l d ',textBoxVars['DecPressure'])).grid(row=6,sticky=Tkinter.E)
buttons['ZeroPoint'] = Tkinter.Button(top, text='Zero Point',command=lambda: executeCommand('z\n')).grid(row=7,sticky=Tkinter.E)
buttons['Turn'] = Tkinter.Button(top, text='Turn',command=lambda: executeCommand('t y\n')).grid(row=8,sticky=Tkinter.E)
buttons['NoTurn'] = Tkinter.Button(top, text='NoTurn',command=lambda: executeCommand('t n\n')).grid(row=9,sticky=Tkinter.E)
buttons['Pause'] = Tkinter.Button(top, text='Pause',command=lambda: executeCommand('p\n')).grid(row=10,sticky=Tkinter.E)
buttons['ActuatorLabel'] = Tkinter.Label(top, text='Actuator Idx').grid(row=11, column=0, sticky=Tkinter.E)

# create text widget
txt = Tkinter.Text(top, borderwidth=3, relief="sunken", width=60,height=5)
txt.config(font=("consolas", 12), undo=True, wrap='word')
txt.grid(row=13, column=0, columnspan=2, sticky="nsew", padx=2, pady=2)

# create a Scrollbar and associate it with txt
scrollb = Tkinter.Scrollbar(top, command=txt.yview)
scrollb.grid(row=11, column=3, sticky='nsew')
txt['yscrollcommand'] = scrollb.set
# txt['state'] = Tkinter.DISABLED

# for k in buttons:
#     buttons[k].pack()

top.mainloop()

serser.close()             # close port