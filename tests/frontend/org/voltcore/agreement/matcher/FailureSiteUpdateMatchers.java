/* This file is part of VoltDB.
 * Copyright (C) 2008-2013 VoltDB Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

package org.voltcore.agreement.matcher;

import static org.hamcrest.Matchers.arrayContainingInAnyOrder;
import static org.hamcrest.Matchers.equalTo;

import java.util.List;
import java.util.Map;

import org.hamcrest.Description;
import org.hamcrest.Matcher;
import org.hamcrest.TypeSafeMatcher;
import org.voltcore.messaging.FailureSiteForwardMessage;
import org.voltcore.messaging.FailureSiteUpdateMessage;
import org.voltcore.utils.Pair;

import com.google.common.collect.ImmutableMap;
import com.google.common.primitives.Longs;
import com.natpryce.makeiteasy.Donor;

public class FailureSiteUpdateMatchers {
    static public final Matcher<FailureSiteUpdateMessage> failureUpdateMsgIs(
            final long site, final long txnid) {
        return new TypeSafeMatcher<FailureSiteUpdateMessage>() {

            @Override
            public void describeTo(Description d) {
                d.appendText("FailureSiteUpdateMessage [ ")
                .appendText("failedSite: ").appendValue(site)
                .appendText(", txnid: ").appendValue(txnid)
                .appendText("]");
            }

            @Override
            protected boolean matchesSafely(FailureSiteUpdateMessage m) {
                return equalTo(site).matches(m.m_failedHSId)
                    && equalTo(txnid).matches(m.m_safeTxnId);

            }
        };
    }

    static public final Matcher<FailureSiteUpdateMessage> failureUpdateMsgIs(
            final long txnid, final Donor<List<Pair<Long, Boolean>>> hsids) {
        return new TypeSafeMatcher<FailureSiteUpdateMessage>() {
            final Map<Long,Boolean> m_hsids;
            {
                ImmutableMap.Builder<Long, Boolean> imb = ImmutableMap.builder();
                for (Pair<Long,Boolean> p: hsids.value()) {
                    imb.put(p.getFirst(),p.getSecond());
                }
                m_hsids = imb.build();
            }

            @Override
            public void describeTo(Description d) {
                d.appendText("FailureSiteUpdateMessage [ ")
                .appendText(", txnid: ").appendValue(txnid)
                .appendText(", failed hsids: ").appendValue(m_hsids)
                .appendText("]");
            }

            @Override
            protected boolean matchesSafely(FailureSiteUpdateMessage m) {
                return equalTo(txnid).matches(m.m_safeTxnId)
                    && equalTo(m_hsids).matches(m.m_failedHSIds);
            }
        };
    }

    static public final Matcher<FailureSiteUpdateMessage> failureUpdateMsgIs(
            final long site, final long txnid, final Donor<List<Pair<Long, Boolean>>> hsids) {
        return new TypeSafeMatcher<FailureSiteUpdateMessage>() {
            final Map<Long,Boolean> m_hsids;
            {
                ImmutableMap.Builder<Long, Boolean> imb = ImmutableMap.builder();
                for (Pair<Long,Boolean> p: hsids.value()) {
                    imb.put(p.getFirst(),p.getSecond());
                }
                m_hsids = imb.build();
            }

            @Override
            public void describeTo(Description d) {
                d.appendText("FailureSiteUpdateMessage [ ")
                .appendText("failedSite: ").appendValue(site)
                .appendText(", txnid: ").appendValue(txnid)
                .appendText(", failed hsids: ").appendValue(m_hsids)
                .appendText("]");
            }

            @Override
            protected boolean matchesSafely(FailureSiteUpdateMessage m) {
                return equalTo(site).matches(m.m_failedHSId)
                    && equalTo(txnid).matches(m.m_safeTxnId)
                    && equalTo(m_hsids).matches(m.m_failedHSIds);
            }
        };
    }

    static public final Matcher<FailureSiteForwardMessage> failureForwardMsgIs(
            final long reportingHsid,
            final long txnid,
            final Donor<List<Pair<Long, Boolean>>> hsids) {

        return new TypeSafeMatcher<FailureSiteForwardMessage>() {

            final Matcher<FailureSiteUpdateMessage> fsumIs =
                    failureUpdateMsgIs(txnid,hsids);

            @Override
            public void describeTo(Description d) {
                d.appendText("FailureSiteForwardMessage [ ")
                .appendText("reportingSite: ").appendValue(reportingHsid)
                .appendText(", ").appendDescriptionOf(fsumIs)
                .appendText(" ]");
            }

            @Override
            protected boolean matchesSafely(FailureSiteForwardMessage m) {
                return fsumIs.matches(m)
                    && equalTo(reportingHsid).matches(m.m_reportingHSId);
            }
        };
    }

    static public final Matcher<long[]> survivorsAre( final long ...survivors) {
        return new TypeSafeMatcher<long[]>() {
            @Override
            public void describeTo(Description d) {
                d.appendValueList("[", ", ", "]", Longs.asList(survivors));
            }
            @Override
            protected boolean matchesSafely(long[] a) {
                return arrayContainingInAnyOrder(survivors).matches(a);
            }
        };
    }
}