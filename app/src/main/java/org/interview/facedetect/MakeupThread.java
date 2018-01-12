package org.interview.facedetect;

import java.util.*;

import org.interview.jni.MakeupJni;

import android.util.Log;

public class MakeupThread implements Runnable {
    private final Object mSync = new Object();
    private final ArrayList<Runnable> mRunnableQueue = new ArrayList<Runnable>();
    private boolean mShutdown;
    private Thread mThread;

    public MakeupThread() {
        mShutdown = false;
    }

    public void pushQueue(Runnable r) {
        synchronized (mSync) {
            mRunnableQueue.add(r);
            mSync.notify();
        }
    }

    public void clearQueue() {
        synchronized (mSync) {
            mRunnableQueue.clear();
        }
    }

    public void startProcess() {
        mThread = new Thread(this);
        mThread.start();
    }

    public void stopProcess() {
        mShutdown = true;
        synchronized (mSync) {
            mSync.notifyAll();
        }
        if(mThread!=null && mThread.isAlive()){
            try {
                mThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public void run() {
        MakeupJni.init();
        Log.e("MY_LOG_TAG", "init-------");
        while (true) {
            Runnable event = null;
            synchronized (mSync) {
                if(mRunnableQueue.isEmpty()){
                    try {
                      mSync.wait();
                    } catch (InterruptedException e) {
                      e.printStackTrace();
                    }
                }
                if(mRunnableQueue.size() > 0){
                    event = mRunnableQueue.remove(0);
                }
                if(null != event){
                    event.run();
                    event = null;
                }
                if (mShutdown) {
                    mRunnableQueue.clear();
                    MakeupJni.destroy();
                    Log.e("MY_LOG_TAG", "destroy======");
                    return;
                }
            }
        }
    }
}
