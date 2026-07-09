/*

   _____                _               _       _             _
  / ____|              | |             | |     | |           (_)
 | (___    ___    ___  | |__     ___   | |_    | |__     ___   _   ___   ___
  \___ \  / _ \  / _ \ | '_ \   / _ \  | __|   | '_ \   / _ \ | | / __| / _ \
  ____) || (_) | (_) | | | | | | (_) | | |_    | | | | |  __/ | | \__ \|  __/
 |_____/  \___/  \___/ |_| |_|  \___/   \__|   |_| |_|  \___| |_| |___/ \___|

                                                                    Version 5.x
*/



// There is a lot of documentation on this include, but here is the short version:
//
//     foreach (new i : Players)
//     {
//         // Will print "Playerid: 0", "Playerid: 1" etc for all connected players.
//         printf("Playerid: %d", i);
//     }
//
// The syntax is:
//
//     foreach (new iterator : Group)
//     {
//         // Code.
//     }
//
// There is also:
//
//     foreach (Player, i)
//
// Which is shorthand for:
//
//     foreach (new i : Players)
//
// Or:
//
//     foreach (new colour = 0; colour < 100; colour += 5)
//
// For ranges and Iterate.

#define _YSI_@YSI_Storage/y_foreach

#include <YSI_Storage\y_amx>
#include <YSI_Storage\y_remote>



// The macros/symbols for basic iterators.
#define YSI_R_FOREACH_INDICES    1
#define YSI_R_FOREACH_ARRAYS     2
#define YSI_R_FOREACH_UNIQUE     4
#define YSI_R_FOREACH_ALL        8
#define YSI_R_FOREACH_BASIC      (YSI_R_FOREACH_INDICES | YSI_R_FOREACH_ARRAYS)
#define YSI_R_FOREACH_NORMAL     (YSI_R_FOREACH_BASIC | YSI_R_FOREACH_ALL)

// Defined iterator properties.
#define YSI_G_FOREACH_NUMBER     1
#define YSI_G_FOREACH_GROUP      2
#define YSI_G_FOREACH_RANGE      4
#define YSI_G_FOREACH_VOID       8
#define YSI_G_FOREACH_ARRAY      16
#define YSI_G_FOREACH_VECTOR     32
#define YSI_G_FOREACH_PBK        (-1)
#define YSI_G_FOREACH_PBZ        (-2)
#define YSI_G_FOREACH_PB0        (-3)
#define YSI_G_FOREACH_PBS        (-4)
#define YSI_G_FOREACH_SLOT       (-5)

// These are the different types of iterator.
#define YSI_G_FOREACH_@Manual       0
#define YSI_G_FOREACH_@YSI_Static   1
#define YSI_G_FOREACH_@YSI_Dynamic  2

// For YSI_ITERATE_R_ITERATE_LOOP.
#if !defined YSI_ITERATE_R_ITERATE_LOOP
	#define YSI_ITERATE_R_ITERATE_LOOP 0
#endif

// For YSI_ITERATE_R_ITERATE_LOOP_SKIP.
#if !defined YSI_ITERATE_R_ITERATE_LOOP_SKIP
	#define YSI_ITERATE_R_ITERATE_LOOP_SKIP 0
#endif



enum E_ITERATOR_ARRAY_DATA
{
	E_ITERATOR_ARRAY_DATA_INTERNAL[1]
}

enum E_ITERATOR_SLOT_DATA
{
	E_ITERATOR_SLOT_DATA_INTERNAL[1]
}



#define Iterator@(%0)     Iterator@%0



// For functions we call (rather than inline) in both remote and not-remote
// modes.
#define _FOREACH_REMOTE_LOCAL@Cyt0(%1) \
	YSI_Remote_Internal_Call@Cyt0(%1)

#define _FOREACH_REMOTE_LOCAL@DCyt0(%1) \
	YSI_Remote_Internal_Call@DCyt0(%1)

#define _FOREACH_REMOTE_LOCAL@Cyt(%1,%2) \
	YSI_Remote_Internal_Call@Cyt(%1,%2)

#define _FOREACH_REMOTE_LOCAL@DCyt(%1,%2) \
	YSI_Remote_Internal_Call@DCyt(%1,%2)

#define _FOREACH_REMOTE_LOCAL@Cyt1(%1,%2,%3) \
	YSI_Remote_Internal_Call@Cyt1(%1,%2,%3)

#define _FOREACH_REMOTE_LOCAL@DCyt1(%1,%2,%3) \
	YSI_Remote_Internal_Call@DCyt1(%1,%2,%3)



/*

   _____      _                        _                   _
  / ____|    | |                      | |                 | |
 | (___   ___| |_ _   _ _ __ ___  ___| |_ _ __ ___  _ __ | | ___
  \___ \ / _ \ __| | | | '__/ _ \/ __| __| '__/ _ \| '_ \| |/ _ \
  ____) |  __/ |_| |_| | | |  __/ (__| |_| | | (_) | | | | |  __/
 |_____/ \___|\__|\__,_|_|  \___|\___|\__|_|  \___/|_| |_|_|\___|



*/



// Some general utilities.

/*
 *  Returns true of the cell is one of the "invalid" values of "cellmin" etc.
 */
#define _FOREACH_@Cx(%0) \
	(((unsigned _:(%0)) + 1) < 2)

/*
 *  Gets the slot and bit of a given index in a bitset array.
 */
#define _FOREACH_@Cxb14(%0,%1,%2) \
	%1 = (%0 >>> %2) & CellbitsInMinusOne

#define _FOREACH_@Cxb9(%0,%1) \
	_FOREACH_@Cxb14(%0,%1,5)

#define _FOREACH_@Cxb1(%0,%1) \
	_FOREACH_@Cxb9(%0,%1), %0 &= 31

#define _FOREACH_@Cxb(%0,%1,%2) \
	_FOREACH_@Cxb1(%1,%2)

/*
 *  Gets the slot and bit of a given index in a bitset array.
 */
#define _FOREACH_@Cxh14(%0,%1,%2) \
	%1 = (%0 % (1 << %2))

#define _FOREACH_@Cxh9(%0,%1) \
	_FOREACH_@Cxh14(%0,%1,5)

//#define _FOREACH_@Cxh(%0,%1) \
//	_FOREACH_@Cxh9(%0,%1)

/*
 *  Gets the slot and bit of a given index in a bitset array.
 */
#define _FOREACH_@CxD(%0) \
	((%0) >>> 5)

#define _FOREACH_@Cxb3(%0,%1) \
	_FOREACH_@CxD(%0), 1 << (%0 & 31), %1

/*
 *  For the address of a variable.
 */
#define _FOREACH_@Cx&(%0) \
	_:%0

/*
 *  The size of a static array.
 */
#define _FOREACH_@CxSz(%0) \
	sizeof(%0)

/*
 *  Multi-dimensional array size.
 */
#define _FOREACH_@CxS0(%0) \
	sizeof(%0)>>>1

/*
 *  Multi-dimensional array size.
 */
#define _FOREACH_@CxS1(%0) \
	sizeof(%0)

/*
 *  Number of cells in a variable (for iterator array allocations).
 */
#define _FOREACH_@CxVc(%0) \
	_:((sizeof(%0)))

/*
 *  Number of cells in a variable (for iterator array allocations).  This is the
 *  same as `_FOREACH_@CxVc` but takes an array and an index, in which case it
 *  always returns 1.
 */
#define _FOREACH_@CxVc1(%0,%1) \
	_:1

/*
 *  Get the array data from an iterator.
 */
#define _FOREACH_@CxAr(%0) \
	_:%0

/*
 *  Get the main array data from an iterator.
 */
#define _FOREACH_@CxAr2(%0) \
	_:%0

/*
 *  Check for an empty slot.
 */
#define _FOREACH_@CxEm(%0) \
	!%0

/*
 *  Unlikely branch.
 */
#if __Pawn < 0x0400
	#define _FOREACH_@CxUn(%0) \
		%0
#else
	#define _FOREACH_@CxUn(%0) \
		__builtin_unlikely(%0)
#endif

/*
 *  While.
 */
#define _FOREACH_@while%0||%1||%2||%3||%4 \
	while (%0(%1, %2, %3, %4))

// Code to yield cells.
#define _FOREACH_@CxYe(%0) \
	_:%0



/*

       _                         _   _
      (_)                       | | (_)
  ___  _  _ __   ___   ___  __ _| |_ _  ___  _ __  ___
 / __| | | '_ \ / __| / __|/ _` | __| |/ _ \| '_ \/ __|
 \__ \ | | | | | (__  \__ \ (_| | |_| | (_) | | | \__ \
 |___/ |_|_| |_|\___| |___/\__,_|\__|_|\___/|_| |_|___/



*/



/*
 *  These functions manipulate bitsets.  They are similar to the functions in
 *  YSI and Y Iterators, but have slightly different names to avoid conflicts.
 *  But they are internal and should never be called directly.
 */

/*
 *  Gets the first set bit from a slot, or -1.
 */
stock _FOREACH_@CxCtN(%0)
{
	if (%0)
	{
		new
			i = 0;
		while (!(%0 & (1 << i)))
		{
			++i;
		}
		return i;
	}
	return -1;
}

/*
 *  Gets the next set bit after the current position.
 */
stock _FOREACH_@CxCtS(%0, %1)
{
	%0 >>= ++%1;
	if (%0)
	{
		while (!(%0 & 1))
		{
			%0 >>= 1;
			++%1;
		}
		return %1;
	}
	return -1;
}

/*
 *  Gets the first set bit in a bitset array.
 */
stock _FOREACH_@CxFn(%0[], %1 = 0)
{
	while (%1 >= 0)
	{
		new
			bits = %0[%1];
		if (bits)
		{
			return %1 * 32 + _FOREACH_@CxCtN(bits);
		}
		--%1;
	}
	return -1;
}

/*
 *  Gets the last set bit in a bitset array.
 */
stock _FOREACH_@CxLn(%0[], %1 = 0)
{
	while (%1 >= 0)
	{
		new
			bits = %0[%1];
		if (bits)
		{
			new
				pos = 31;
			while (!(bits & (1 << pos)) && pos >= 0)
			{
				--pos;
			}
			return %1 * 32 + pos;
		}
		--%1;
	}
	return -1;
}

/*
 *  Gets the first set bit in a bitset array.
 */
stock _FOREACH_@CxFnForward(%0[], %1 = 0, %2)
{
	while (%1 < %2)
	{
		new
			bits = %0[%1];
		if (bits)
		{
			return %1 * 32 + _FOREACH_@CxCtN(bits);
		}
		++%1;
	}
	return -1;
}

/*
 *  Clears a specific bit in a bitset array.
 */
stock _FOREACH_@CxClrBit(%0[], %1)
{
	new
		slot = %1 >>> 5;
	%0[slot] &= ~(1 << (%1 & 31));
}

/*
 *  Sets a specific bit in a bitset array.
 */
stock _FOREACH_@CxSetBit(%0[], %1)
{
	new
		slot = %1 >>> 5;
	%0[slot] |= (1 << (%1 & 31));
}

/*
 *  Gets a specific bit from a bitset array.
 */
stock _FOREACH_@CxGetBit(%0[], %1)
{
	new
		slot = %1 >>> 5;
	return _FOREACH_@CxUn(%0[slot] & (1 << (%1 & 31)));
}

/*
 *  Gets the last set bit in a bitset array.
 */
stock _FOREACH_@CxLnBackward(%0[], %1 = 0, %2)
{
	while (%1 >= %2)
	{
		new
			bits = %0[%1];
		if (bits)
		{
			new
				pos = 31;
			while (!(bits & (1 << pos)) && pos >= 0)
			{
				--pos;
			}
			return %1 * 32 + pos;
		}
		--%1;
	}
	return -1;
}

/*
 *  Counts the number of set bits.
 */
stock _FOREACH_@CxCt(%0[], %1 = 0)
{
	new
		count = 0;
	while (%1 >= 0)
	{
		new
			bits = %0[%1];
		while (bits)
		{
			bits &= (bits - 1);
			++count;
		}
		--%1;
	}
	return count;
}

/*
 *  Copy bits.
 */
stock _FOREACH_@CxCp(%0[], %1[], %2)
{
	memcpy(%0, %1, 0, %2 * 4, 4);
}

/*
 *  Clear all bits.
 */
stock _FOREACH_@CxCl(%0[], %1)
{
	while (%1 >= 0)
	{
		%0[%1] = 0;
		--%1;
	}
}

/*
 *  Initialise all bits to set.
 */
stock _FOREACH_@CxIn(%0[], %1)
{
	while (%1 >= 0)
	{
		%0[%1] = cellmax;
		--%1;
	}
}

/*
 *  How many cells needed for a given number of bits?
 */
stock _FOREACH_@CxNc(%0)
{
	return (%0 + 31) >>> 5;
}



/*

  __  __                          _
 |  \/  |                        | |
 | \  / |  __ _  _ __   ___    __| |  ___  _ __
 | |\/| | / _` || '_ \ / __|  / _` | / _ \| '__|
 | |  | || (_| || | | |\__ \ | (_| ||  __/| |
 |_|  |_| \__,_||_| |_||___/  \__,_| \___||_|



*/



/*
 *  Internal variable to register an enum.
 */
#define _FOREACH_ENUM_REGISTRY%0\32;%1\32; \
	_FOREACH_ENUM_REGISTRY_REAL%0\32;%1\32;

/*
 *  This is used to store the raw ENUM data.
 */
#define _FOREACH_ENUM_REGISTRY_REAL%0\32;%1\32; \
	enum _FOREACH_ENUM_REGISTRY_ENUM_%0 (<<= 1) \
	{ \
		_FOREACH_ENUM_REGISTRY_ENUM_%0 = %1 \
	}; \
	_FOREACH_ENUM_REGISTRY%0,%1

/*
 *  Storage hook.
 */
#define _FOREACH_ENUM_REGISTRY%0,%1

/*
 *  These are the default enum tags.
 */
#define UNARY_LOD%0[%1]  %0[%1]



/*

       _                            _
      (_)                          | |
  ___  _  _ __    ___  _ __   ___  | |_  ___
 / __|| || '_ \  / _ \| '_ \ / _ \ | __|/ _ \
 \__ \| || |_) ||  __/| | | | (_) || |_|  __/
 |___/|_|| .__/  \___||_| |_|\___/  \__|\___|
         | |
         |_|



*/



/*
 *  For later use.
 */
#if !defined _FOREACH_@CxNc
	#define _FOREACH_@CxNc(%0) \
		(((%0) + 31) >>> 5)
#endif



/*

  __  __       _        _                                    _   _
 |  \/  |     | |      | |                                  | | (_)
 | \  / |_   _| |_ __ _| |__   __ _ ___  ___    ___  _ __ __| |  _  _ __   ___
 | |\/| | | | | __/ _` | '_ \ / _` / __|/ _ \  / _ \| '__/ _` | | || '_ \ / __|
 | |  | | |_| | || (_| | |_) | (_| \__ \  __/ | (_) | | | (_| | | || | | |\__ \
 |_|  |_ \__, |\__\__,_|_.__/ \__,_|___/\___|  \___/|_|  \__,_| |_||_| |_||___/
          __/ |
         |___/
*/


/*
 *  A structure used to store infinite loop variables.
 */
stock const _FOREACH_@CxInfiniteArray[1];



/*

      _               _             _                    _
     | |             (_)           | |                  | |
   __| | _ __   __ _  _  _ __    __| |  ___  _ __   ___ | |_
  / _` || '__| / _` || || '_ \  / _` | / _ \| '_ \ / __|| __|
 | (_| || |   | (_| || || | | || (_| ||  __/| | | |\__ \| |_
  \__,_||_|    \__,_||_||_| |_| \__,_| \___||_| |_||___/ \__|



*/



/*
 *  The standard loops.
 */

/*
 *  foreach (new i : Players)
 *
 *  Very simple loop, iterates over all the connected players.
 *
 *  Current limitations:\
 *  - Cannot be nested.
 *
 *  Future features:
 *  - Make it nested.
 */
#define _FOREACH_LOCAL_LOOP_0_11(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_12(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_12(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_13(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_13(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_14(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_14(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_15(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_15(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_16(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_16(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_17(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_17(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_18(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_18(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_19(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_19(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_20(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_20(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_21(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_21(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_22(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_22(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_23(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_23(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_24(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_24(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_25(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_25(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_26(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_26(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_27(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_27(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_28(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_28(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_29(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_29(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_30(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_30(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_31(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_31(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_32(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_32(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_33(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_33(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_34(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_34(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_35(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_35(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_36(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_36(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_37(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_37(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_38(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_38(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_39(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_39(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_40(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_40(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_41(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_41(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_42(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_42(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_43(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_43(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_44(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_44(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_45(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_45(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_46(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_46(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_47(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_47(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_48(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_48(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_49(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_49(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_50(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_50(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_51(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_51(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_52(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_52(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_53(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_53(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_54(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_54(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_55(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_55(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_56(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_56(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_57(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_57(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_58(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_58(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_59(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_59(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_60(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_60(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_61(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_61(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_62(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_62(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_63(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_63(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_64(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_64(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_65(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_65(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_66(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_66(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_67(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_67(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_68(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_68(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_69(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_69(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_70(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_70(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_71(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_71(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_72(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_72(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_73(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_73(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_74(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_74(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_75(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_75(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_76(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_76(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_77(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_77(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_78(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_78(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_79(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_79(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_80(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_80(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_81(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_81(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_82(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_82(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_83(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_83(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_84(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_84(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_85(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_85(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_86(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_86(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_87(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_87(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_88(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_88(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_89(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_89(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_90(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_90(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_91(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_91(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_92(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_92(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_93(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_93(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_94(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_94(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_95(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_95(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_96(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_96(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_97(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_97(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_98(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_98(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_99(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_99(%0,%1,%2,%3,%4,%5) \
	_FOREACH_LOCAL_LOOP_0_100(%0,%1,%2,%3,%4,%5)

#define _FOREACH_LOCAL_LOOP_0_100(%0,%1,%2,%3,%4,%5)



/*

       _               _            _                   _
      | |             | |          | |                 | |
   ___| |__   ___  ___| | ________ | |     ___  ___  __| |
  / __| '_ \ / _ \/ __| |/ /______| |    / _ \/ _ \/ _` |
 | (__| | | |  __/ (__|   <       | |___|  __/  __/ (_| |
  \___|_| |_|\___|\___|_|\_\       \_____/\___|\___|\__,_|



*/



// Now we have to include the data files.



#include "foreach.y_files/foreach.y_0"
#include "foreach.y_files/foreach.y_1"
#include "foreach.y_files/foreach.y_2"
#include "foreach.y_files/foreach.y_3"
#include "foreach.y_files/foreach.y_4"
#include "foreach.y_files/foreach.y_5"



/*

  __  __       _        _                              _   _
 |  \/  |     | |      | |                            | | (_)
 | \  / |_   _| |_ __ _| |__   __ _ ___  ___    _   _ | |_ _  ___  _ __
 | |\/| | | | | __/ _` | '_ \ / _` / __|/ _ \  | | | || __| |/ _ \| '_ \
 | |  | | |_| | || (_| | |_) | (_| \__ \  __/  | |_| || |_| | (_) | | | |
 |_|  |_ \__, |\__\__,_|_.__/ \__,_|___/\___|   \__,_| \__|_|\___/|_| |_|
          __/ |
         |___/



*/



/*
 *  Library data functions.
 */

/*
 *  Define a group of small iterators.
 */
#define DefineFunctionGroup@Cx(%0) \
	forward _FOREACH_FUNCTION_GROUP@Cx(%0, 1, 0); \
	stock _FOREACH_FUNCTION_GROUP@Cx(%0, 1, 0) {}

/*
 *  Define a group of small iterators.
 */
#define DefineIteratorGroup@Cx(%0) \
	forward _FOREACH_ITERATOR_GROUP@Cx(%0, 1, 0); \
	stock _FOREACH_ITERATOR_GROUP@Cx(%0, 1, 0) {}

/*
 *  Define a variable.
 */
#define DefineHook@Cx(%0) \
	forward _FOREACH_HOOK@Cx(%0); \
	stock _FOREACH_HOOK@Cx(%0) {}



/*

      _       _                     _   _
     (_)     | |                   | | (_)
  ___ _  __ _| |__   ___  _ __ __ _| |_ _  ___  _ __  ___
 / __| |/ _` | '_ \ / _ \| '__/ _` | __| |/ _ \| '_ \/ __|
 \__ \ | (_| | | | | (_) | | | (_| | |_| | (_) | | | \__ \
 |___/_|\__, |_| |_|\___/|_|  \__,_|\__|_|\___/|_| |_|___/
         __/ |
        |___/



*/



/*
 *  Compile-time iteration.
 */
#define _FOREACH_ITERATE_INTERNAL@ITERATE(...) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_INNER(__VA_ARGS__)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_INNER(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_1(%0=,%1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_1(%0=%1,%2) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_2(%0,%1,%2)

//#define _FOREACH_ITERATE_INTERNAL@ITERATE_2(%0=%1,%2,%3)

/*
 *  Macro recursion macros.
 */
#define _FOREACH_ITERATE_INTERNAL@ITERATE_2(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_3(%0, %1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_3(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_4(%0, %1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_4(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_5(%0, %1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_5(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_6(%0, %1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_6(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_7(%0, %1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_7(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_8(%0, %1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_8(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_9(%0, %1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_9(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_10(%0, %1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_10(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_11(%0, %1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_11(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_12(%0, %1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_12(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_RESTART(%0, %1)

#define _FOREACH_ITERATE_INTERNAL@ITERATE_RESTART(%0,%1) \
	_FOREACH_ITERATE_INTERNAL@ITERATE_2(%0, %1)



/*

  __  __       _        _                         _   _
 |  \/  |     | |      | |                       | | (_)
 | \  / |_   _| |_ __ _| |__   __ _ ___  ___   __| |_ _ _ __   __ _
 | |\/| | | | | __/ _` | '_ \ / _` / __|/ _ \ / _` | | '_ \ / _` |
 | |  | | |_| | || (_| | |_) | (_| \__ \  __/| (_| | | | | | (_| |
 |_|  |_ \__, |\__\__,_|_.__/ \__,_|___/\___| \__,_|_|_| |_|\__, |
          __/ |                                               __/ |
         |___/                                               |___/



*/



// Empty placeholder for the end of the file.
#define _FOREACH_LOCAL_LOOP_0_101(%0,%1,%2,%3,%4,%5)

//EOF
