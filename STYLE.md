# tomb5 style guide

## Tabs

## No braces for single lined ifs, for loops, while loops, etc.

eg:

```
if (room_number != item->room_number)
	ItemNewRoom(item_number, room_number);
```

## Spaces before and after if statements, for loops, while loops, switch/case blocks

## Braces need to be on their own lines

eg:

```
if (camera.item && (camera.type == FIXED_CAMERA || camera.type == HEAVY_CAMERA))
		fixed_camera = 1;
	else
	{
		item = lara_item;
		fixed_camera = 0;
	}
```

## Single line do-while loops can be on the same line

eg:

```
do AnimateLara(l); while (l->current_anim_state != AS_NULL);
```

## ++ or -- should be a suffix, only a prefix when necessary

## Try to keep all the locals at the top of said functions

Exceptions are the control vars in for loops.

eg:

```
for (int i = 0; i < num; i++)
{
	stuff
	stufffff
}
```

## For loops

```
for (int i = 0; i < num; i++)
```

For nested loops, first name to use is i, then j, then you are free to choose.
UNLESS the value is used elsewhere after the loop, then any name is fine, and the variable should be at the top of the function.

## Order of variables at the top of functions

struct pointers
structs
struct arrays
float pointers
long pointers
ulong pointers
short pointers
ushort pointers
char pointers
uchar pointers
float arrays
floats
long arrays
longs
short arrays
shorts
char arrays
chars

## Switch/case formatting

- Empty spaces between each case
- Empty spaces between the case and the line that follows if that line is an if statement, for loop, while loop, or another switch/case

example:

```
switch (x)
{
	case 0:
		x++;
		break;

	case 1:

		if (x == 2)
			x--;

		break;

	case 2:
		x /= 2;
		break;

	case 3:
		x = y;

	case 4:
		x += z;
		break;

	default:
		abort();
		break;
}
```
