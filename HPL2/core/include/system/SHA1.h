/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * SHA1.h
 *
 *  Copyright (C) 1998, 2009
 *  Paul E. Jones <paulej@packetizer.com>
 *  All Rights Reserved.
 *
 *  Intregration into HPL2 engine and using std string / wstring input and outputs
 *  Edward Rudd <urkle@outoforder.cc>
 *
 *  Description:
 *      This class implements the Secure Hashing Standard as defined
 *      in FIPS PUB 180-1 published April 17, 1995.
 *
 *      Many of the variable names in this class, especially the single
 *      character names, were used because those were the names used
 *      in the publication.
 *
 *      Please read the file sha1.cpp for more information.
 *
 */

#ifndef SHA1_H_
#define SHA1_H_

#include "system/SystemTypes.h"
#include <string>

namespace hpl {

class SHA1 {
public:
	SHA1();
	virtual ~SHA1();

	/*
	 *  Reset
	 *
	 *  Description:
	 *      This function will initialize the sha1 class member variables
	 *      in preparation for computing a new message digest.
	 *
	 *  Parameters:
	 *      None.
	 *
	 *  Returns:
	 *      Nothing.
	 *
	 *  Comments:
	 *
	 */
    void Reset();

    static SHA1 &reset(SHA1&);

	/*
	 *  Result
	 *
	 *  Description:
	 *      This function will return the 160-bit message digest into the
	 *      array provided.
	 *
	 *  Parameters:
	 *      message_digest_array: [out]
	 *          This is an array of five unsigned integers which will be filled
	 *          with the message digest that has been computed.
	 *
	 *  Returns:
	 *      True if successful, false if it failed.
	 *
	 *  Comments:
	 *
	 */
    bool Result(unsigned *message_digest_array);
	/*
	 *  Result
	 *
	 *  Description:
	 *  	This function will return a hex digest for the SHA1
	 *
	 *  Parameters:
	 *  	hex: [out]
	 *  		The std string to output the hex data into
	 *
	 *  Returns:
	 *  	True if successful, false if it failed
	 *
	 *  Comments:
	 *
	 */
    bool Result(std::string &hex);

    SHA1& operator>>(tString &hex);
    SHA1& operator>>(tWString &hex);

	/*
	 *  Input
	 *
	 *  Description:
	 *      This function accepts an array of octets as the next portion of
	 *      the message.
	 *
	 *  Parameters:
	 *      message_array: [in]
	 *          An array of characters representing the next portion of the
	 *          message.
	 *
	 *  Returns:
	 *      Nothing.
	 *
	 *  Comments:
	 *
	 */
    void Input( const unsigned char *message_array,
                unsigned            length);
    void Input(const tString &message);
    void Input(const tWString &message);
    SHA1& operator<<(tString message);
    SHA1& operator<<(tWString message);
    SHA1& operator<<(SHA1& (*__mf)(SHA1&));

private:

	/*
	 *  ProcessMessageBlock
	 *
	 *  Description:
	 *      This function will process the next 512 bits of the message
	 *      stored in the Message_Block array.
	 *
	 *  Parameters:
	 *      None.
	 *
	 *  Returns:
	 *      Nothing.
	 *
	 *  Comments:
	 *      Many of the variable names in this function, especially the single
	 *      character names, were used because those were the names used
	 *      in the publication.
	 *
	 */
    void ProcessMessageBlock();

	/*
	 *  PadMessage
	 *
	 *  Description:
	 *      According to the standard, the message must be padded to an even
	 *      512 bits.  The first padding bit must be a '1'.  The last 64 bits
	 *      represent the length of the original message.  All bits in between
	 *      should be 0.  This function will pad the message according to those
	 *      rules by filling the message_block array accordingly.  It will also
	 *      call ProcessMessageBlock() appropriately.  When it returns, it
	 *      can be assumed that the message digest has been computed.
	 *
	 *  Parameters:
	 *      None.
	 *
	 *  Returns:
	 *      Nothing.
	 *
	 *  Comments:
	 *
	 */
    void PadMessage();

	/*
	 *  CircularShift
	 *
	 *  Description:
	 *      This member function will perform a circular shifting operation.
	 *
	 *  Parameters:
	 *      bits: [in]
	 *          The number of bits to shift (1-31)
	 *      word: [in]
	 *          The value to shift (assumes a 32-bit integer)
	 *
	 *  Returns:
	 *      The shifted value.
	 *
	 *  Comments:
	 *
	 */
    inline unsigned CircularShift(int bits, unsigned word);

    unsigned H[5];                      // Message digest buffers

    unsigned Length_Low;                // Message length in bits
    unsigned Length_High;               // Message length in bits

    unsigned char Message_Block[64];    // 512-bit message blocks
    int Message_Block_Index;            // Index into message block array

    bool Computed;                      // Is the digest computed?
    bool Corrupted;                     // Is the message digest corrupted?

};

}

#endif /* SHA1_H_ */
