//
//  GLESView.h
//  Window
//
//  Created by ROHAN WAGHMODE on 10/03/20.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface GLESView : UIView <UIGestureRecognizerDelegate>
-(void)startAnimation;
-(void)stopAnimation;
@end

NS_ASSUME_NONNULL_END
