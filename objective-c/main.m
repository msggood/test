#import <Foundation/Foundation.h>

int main(void)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];    

    NSLog(@"Hello, world!");

    unichar c = u'加';

    NSLog(@"The character is: %C", c);

    [pool drain];
}

