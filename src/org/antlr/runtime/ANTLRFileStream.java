/*
 [The "BSD licence"]
 Copyright (c) 2004 Terence Parr
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
    derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package org.antlr.runtime;

import java.io.IOException;
import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;

// TODO: this should be special case of ANTLRStringStream right?
public class ANTLRFileStream implements CharStream {
    protected int p=0;
    String fileName;

	/** line number 1..n within the input */
	protected int line = 1;

	/** The index of the character relative to the beginning of the line 0..n-1 */
	protected int charPositionInLine = 0;

    char[] data;

	public ANTLRFileStream() {
	}

	public ANTLRFileStream(String fileName) throws IOException {
		load(fileName);
	}

	public void load(String fileName) throws IOException {
		//System.out.println("loading "+fileName);
		this.fileName = fileName;
		FileReader fr = null;
		try {
			File f = new File(fileName);
			data = new char[(int)f.length()];
			fr = new FileReader(fileName);
			fr.read(data);
		}
		finally {
			if ( fr!=null ) {
				fr.close();
			}
		}
    }

	public void reset() {
		line = 1;
		charPositionInLine = 0;
	}

    public void consume() {
        if ( p < data.length ) {
			charPositionInLine++;
			if ( data[p]=='\n' ) {
				line++;
				charPositionInLine=0;
			}
            p++;
        }
    }

    public int LA(int i) {
        if ( (p+i-1) >= data.length ) {
            //System.out.println("char LA("+i+")=EOF; p="+p);
            return CharStream.EOF;
        }
        //System.out.println("char LA("+i+")="+data.charAt(p+i-1)+"; p="+p);
        return data[p+i-1];
    }

    public int LA(int marker, int i) {
        return data[marker+i-1];
    }

    public int mark() {
        return index(); // already buffered, just return index
    }

    /** Return the current input symbol index 0..n where n indicates the
     *  last symbol has been read.
     */
    public int index() {
        return p;
    }

	public int size() {
		return data.length;
	}

    public void rewind(int marker) {
        p = marker;
    }

	public String substring(int start, int stop) {
		return new String(data,start,stop-start+1);
	}

	public int getLine() {
		return line;
	}

	public int getCharPositionInLine() {
		return charPositionInLine;
	}
}
