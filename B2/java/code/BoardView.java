package lifegame;

import javax.swing.JPanel;
import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

public class BoardView extends JPanel implements BoardListener,MouseListener,MouseMotionListener{
	private BoardModel model;
	private int N,M,width,height,masu,Statex,Statey;
	
	public BoardView(BoardModel model) {
		this.model = model;
		this.addMouseListener(this);
		this.addMouseMotionListener(this);
		model.addListener(this);
	}
	
	public void mouseClicked(MouseEvent e) {
	}
	public void mouseEntered(MouseEvent e) {
	}
	
	public void mouseExited(MouseEvent e) {
	}
	
	public void mousePressed(MouseEvent e) {
		int x = e.getX();
        int y = e.getY();
        int i=0;
        int j=0;
        if( x<(width-masu*M)/2 || x>(width+masu*M)/2 || y>masu*(N+1) ) {
        	return;
        }
        
        
        // 左端,上端はその一マスにふくまれているものとする
        for (i = 0; i < N; i++) {
            if (y >= i * masu && y < (i + 1) * masu) {
            	Statey = i * masu; // セルの左上の y 座標
                for (j = 0; j < M; j++) {
                	if(x >= ((width-masu*M)/2) + masu*j && x < ((width-masu*M)/2) + masu*(j+1)) {
                		Statex = (j * masu) + (width - masu * M) / 2; // セルの左上の x 座標
                        break;
                	}
                }
                break;
            }
        }
        model.changeCellState(j,i);
	}
	
	public void mouseReleased(MouseEvent e) {
		
	}
	
	public void mouseDragged(MouseEvent e) {
		int x = e.getX();
        int y = e.getY();
        int masu_x=0;
        int masu_y=0;
        if( x<(width-masu*M)/2 || x>(width+masu*M)/2 || y>masu*(N+1) ) {
        	return;
        }
        for (int i = 0; i < N; i++) {
            if (y >= i * masu && y < (i + 1) * masu) {
            	masu_y = i * masu; // セルの左上の y 座標
                for (int j = 0; j < M; j++) {
                	if(x >= ((width-masu*M)/2) + masu*j && x < ((width-masu*M)/2) + masu*(j+1)) {
                		masu_x = (j * masu) + (width - masu * M) / 2; // セルの左上の x 座標
                        break;
                	}
                }
                break;
            }
        }
        if(!(masu_x == Statex && masu_y == Statey)) {
        	for (int i = 0; i < N; i++) {
                if (y >= i * masu && y < (i + 1) * masu) {
                    for (int j = 0; j < M; j++) {
                        if (x >= ((width-masu*M)/2) + masu*j && x < ((width-masu*M)/2) + masu*(j+1)) {
                            // クリックされたセルの状態を変更
                            model.changeCellState(j,i);
                        }
                    }
                }
            }
        	Statex=masu_x;
        	Statey=masu_y;
        }
	}
	
	public void mouseMoved(MouseEvent e) {
	}
	
	@Override
	public void paint(Graphics g) {
		N = model.getRows();
		M = model.getCols();
		width = this.getWidth();
		height = this.getHeight();
		if((height*995/1000)/N > width/(M+2)) {
			masu = width/(M+2);
		}else {
			masu = (height*995/1000)/N;
		}
		super.paint(g);
		for(int i=0;i<N+1;i++) {
			g.drawLine((width-masu*M)/2,i*masu,width-(width-masu*M)/2,i*masu);
		}
		for(int i=0;i<M+1;i++) {
			g.drawLine(i*masu+(width-masu*M)/2, 0, i*masu+(width-masu*M)/2, N*masu);
		}
		
		for(int i=0;i<N;i++) {
			for(int j=0;j<M;j++) {
				boolean cellState = model.isAlive(j, i);
				if(cellState) {
					g.fillRect((width-masu*M)/2+j*masu, i*masu, masu, masu);
				}
			}
		}
	}
	
	@Override
	public void updated(BoardModel model) {
		this.repaint();
	}
	
}


