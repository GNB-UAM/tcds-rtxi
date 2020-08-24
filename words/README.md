### Words with Custom GUI

**Requirements:** None  
**Limitations:** None  

![Words with Custom GUI GUI](words.png)

<!--start-->
<p><b>Words:</b><br>This is the module Words.This module executes the TCDS protocol, when it detects the word it activates the sending of the stimulus.</p>
<!--end-->

#### Input
1. input(0) - IV : Voltage of the received signal

#### Output
1. output(0) - Now : internal system time
2. output(1) - O : flag that activates the sending of the stimulus
3. output(2) - latency : task latency

#### Parameters
1. Bin Time (ns) - detection window time
2. Threshold (V) - minimum voltage to detect a spike
3. Word length - number of bits of the word
4. Word - word to detect

#### States

